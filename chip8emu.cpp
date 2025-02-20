#include "chip8emu.h"
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <QRandomGenerator>
#include <QTime>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDebug>

//private functions
void Chip8Emu::pushStack(u16 _push)
{
    if(SP + 1 <= 16){
        SP++;
        stack[SP - 1] = _push;
    }
    else{
        emit error(STACK_FULL);
    }
}

void Chip8Emu::popStack(u16 *_pop)
{
    if(SP - 1 >= 0){
        *_pop = stack[SP - 1];
        stack[SP - 1] = 0;
        SP--;
    }
    else{
        emit error(STACK_EMPTY);
    }
}

void Chip8Emu::printInstruction(char* _message)
{
    if(params.verbose){
        printf("Instruction: %x, %s\n", instruction, _message);
    }
    if(params.verboseReg){
        for(int i = 0; i < 16; ++i){
            printf("V%x: %x, ", i, V[i]);
        }
        printf("\n");
    }
}

void Chip8Emu::instruction0() //multiple instructions
{
    switch(instruction & 0x00FF){

    case 0xE0: //clear display
    {
        for(int i = 0; i < 64 * 32; ++i){
            display[i] = 0x0;
        }
        emit draw_flag(display);

        printInstruction("clear display");
        break;
    }

    case 0xEE: //return from subroutine
        popStack(&PC);

        printInstruction("return from subroutine");
        break;

    default:
        emit error(UNKNOWN_INSTRUCTION);

        printInstruction("UNKNOWN INSTRUCTION");
        break;
    }
}

void Chip8Emu::instruction1() //jump to NNN
{
    PC = (instruction & 0x0FFF);

    printInstruction("jump");
}

void Chip8Emu::instruction2() //enter subroutine
{
    pushStack(PC);
    PC = (instruction & 0x0FFF);

    printInstruction("enter subroutine");
}

void Chip8Emu::instruction3() //skip if VX == NN
{
    if(V[x] == (instruction & 0x00FF)){
        PC += 2;
    }

    printInstruction("skip if VX == NN");
}

void Chip8Emu::instruction4() //skip if VX != NN
{
    if(V[x] != (instruction & 0x00FF)){
        PC += 2;
    }

    printInstruction("skip if VX != NN");
}

void Chip8Emu::instruction5() //skip if VX == VY
{
    if(V[x] == V[y]){
        PC += 2;
    }

    printInstruction("skip if VX == VY");
}

void Chip8Emu::instruction6() //set VX to NN
{
    V[x] = (instruction & 0x00FF);

    printInstruction("set VX to NN");
}

void Chip8Emu::instruction7() //add NN to VX
{
    V[x] += (instruction & 0x00FF);

    printInstruction("add NN to VX");
}

void Chip8Emu::instruction8() //multiple instructions
{
    switch(instruction & 0x000F){

    case 0x0: //VX = VY
        V[x] = V[y];

        printInstruction("VX = VY");
        break;

    case 0x1: //Binary OR
        V[x] |= V[y];

        printInstruction("VX |= VY");
        break;

    case 0x2: //Binary AND
        V[x] &= V[y];

        printInstruction("VX &= VY");
        break;

    case 0x3: //Logical XOR
        V[x] ^= V[y];

        printInstruction("VX ^= VY");
        break;

    case 0x4: //add VY to VX w/ carry
        V[0xF] = (V[x] + V[y] > 0xFF) ? 0x1 : 0x0;
        V[x] += V[y];

        printInstruction("VX += VY w/ carry");
        break;

    case 0x5: //subtract VX = VX - VY
        V[0xF] = (V[x] > V[y]) ? 0x1 : 0x0;
        V[x] -= V[y];

        printInstruction("VX -= VY w/ borrow");
        break;

    case 0x6: //right shift VX
        if(params.originalInstruction8){
            V[x] = V[y];
        }
        V[0xF] = V[x] & 0x1;
        V[x] >>= 1;

        printInstruction("VX >>= 1");
        break;

    case 0x7: //subtract VX = VY - VX
        V[0xF] = (V[x] < V[y]) ? 0x1 : 0x0;
        V[x] = V[y] - V[x];

        printInstruction("VX = VY - VX");
        break;

    case 0xE: //left shift VX
        if(params.originalInstruction8){
            V[x] = V[y];
        }
        V[0xF] = V[x] & 0x80;
        V[x] <<= 1;

        printInstruction("VX <<= 1");
        break;

    default:
        emit error(UNKNOWN_INSTRUCTION);

        printInstruction("UNKNOWN INSTRUCTION");
        break;
    }
}

void Chip8Emu::instruction9() //skip if VX != VY
{
    if(V[x] != V[y]){
        PC += 2;
    }

    printInstruction("skip VX != VY");
}

void Chip8Emu::instructionA() //set index register
{
    I = (instruction & 0x0FFF);

    printInstruction("I = NNN");
}

void Chip8Emu::instructionB() //jump with offset
{
    PC = (instruction & 0x0FFF) + ((params.originalInstruction8)? V[0] : V[x]);

    printInstruction("jump with offset");
}

void Chip8Emu::instructionC() //VX = random
{
    V[x] = (QRandomGenerator::global()->generate() & (instruction & 0x00FF));

    printInstruction("VX = random");
}

void Chip8Emu::instructionD() //update display
{
    //Get N from instruction (height)
    u8 height = (instruction & 0x000F);
    u8 x_coord = V[x] % 64;
    u8 y_coord = V[y] % 32;

    //Get a byte of pixels
    u8 pixel;

    //Set F register to 0
    V[0xF] = 0;

    //Go through each line for height
    for(int y = 0; y < height; ++y){
        if(y_coord + y > 31){
            break;
        }
        pixel = memory[I + y];

        for(int x = 0; x < 8; ++x){
            //check if the pixel is on
            if(x_coord + x > 63){
                break;
            }
            if((pixel & (0x80 >> x)) != 0x0){
                if(display[x_coord + x + ((y_coord + y) * 64)] != 0x0){
                    V[0xF] = 0x1;
                    display[x_coord + x + ((y_coord + y) * 64)] = 0x0;
                }
                else{
                    display[x_coord + x + ((y_coord + y) * 64)] = 0x1;
                }
            }
        }
    }
    emit draw_flag(display);

    printInstruction("draw sprite");
}

void Chip8Emu::instructionE() //multiple instructions
{
    switch(instruction & 0x00FF){

    case 0x9E: //skip if key pressed
        if(keypad[V[x]]){
            PC += 2;
        }

        printInstruction("skip if key pressed");
        break;

    case 0xA1: //skip if key not pressed
        if(!keypad[V[x]]){
            PC += 2;
        }

        printInstruction("skip if key not pressed");
        break;

    default:
        emit error(UNKNOWN_INSTRUCTION);

        printInstruction("UNKNOWN INSTRUCTION");
        break;
    }
}

void Chip8Emu::instructionF() //multiple instructions
{
    switch(instruction & 0x00FF){

    case 0x07: //VX = delay timer
        V[x] = delay_timer;

        printInstruction("VX = delay timer");
        break;

    case 0x0A: //pause until any key pressed
        PC -= 2;

        printInstruction("wait for key press");

        for(int i = 0; i < 16; ++i){
            if(keypad[i]){
                PC += 2;
                V[x] = i;
                break;
            }
        }
        break;

    case 0x15: //delay timer = VX
        delay_timer = V[x];

        printInstruction("delay timer = VX");
        break;

    case 0x18: //sound timer = VX
        sound_timer = V[x];

        printInstruction("sound timer = VX");
        break;

    case 0x1E: //add VX to index
        if(params.instructionOverflowF && (I + V[x]) >= 0x1000){
            V[0xF] = 0x1;
        }
        I += V[x];

        printInstruction("I += VX");
        break;

    case 0x29: //point to font character
        I = V[x] * 5;

        printInstruction("point I to font");
        break;

    case 0x33: //BDC
        memory[I] = (V[x] % 1000) / 100;
        memory[I + 1] = (V[x] % 100) / 10;
        memory[I + 2] = (V[x] % 10);

        printInstruction("BDC and store");
        break;

    case 0x55: //store memory
        if(params.instructionIncrementF){
            for(int i = 0; i <= x; ++i){
                memory[I++] = V[i];
            }
        }
        else{
            for(int i = 0; i <= x; ++i){
                memory[I + i] = V[i];
            }
        }

        printInstruction("store registers to memory");
        break;

    case 0x65: //load memory
        if(params.instructionIncrementF){
            for(int i = 0; i <= x; ++i){
                V[i] = memory[I++];
            }
        }
        else{
            for(int i = 0; i <= x; ++i){
                V[i] = memory[I + i];
            }
        }

        printInstruction("load registers from memory");
        break;

    default:
        emit error(UNKNOWN_INSTRUCTION);

        printInstruction("UNKNOWN ERROR");
        break;
    }
}

void Chip8Emu::emulate()
{
    while(running){
        //reduce cpu stress while paused
        while(paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        //start timer
        auto start = std::chrono::high_resolution_clock::now();

        //get the current instruction
        instruction = (memory[PC] << 8) | (memory[PC + 1]);

        //grab x and y from instruction
        x = (instruction & 0x0F00) >> 8;
        y = (instruction & 0x00F0) >> 4;

        //increment program counter
        PC += 2;

        //decode and execute instruction
        (this->*instructionTable[(instruction & 0xF000) >> 12])();

        //calculate execution times
        auto end = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        auto remainingTime = emulatorHz - executionTime;

        //wait proper time for next run
        while(remainingTime > 0)
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
            remainingTime = (emulatorHz) - elapsedTime - executionTime;
        }
    }
}

void Chip8Emu::updateTimers()
{
    while(running)
    {
        //reduce cpu stress while paused
        while(paused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        //start timer
        auto start = std::chrono::high_resolution_clock::now();

        //decrement counters
        if(sound_timer > 0)
        {
            sound_timer--;
        }

        if(delay_timer > 0)
        {
            delay_timer--;
        }

        //calculate execution times
        auto end = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        auto remainingTime = timerHz - executionTime;

        //wait proper time for next run
        while(remainingTime > 0)
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            remainingTime = timerHz - elapsedTime - executionTime;
        }
    }
}

void Chip8Emu::resetComponents()
{
    for(int i = 0; i < 64 * 32; ++i){
        display[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        stack[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        V[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        keypad[i] = false;
    }

    PC = 0x200;
    SP = 0x0;
    delay_timer = 0x0;
    sound_timer = 0x0;
}

//public functions
int Chip8Emu::loadROM(const QString _filename)
{
    //stop instance
    this->quit();

    //open the passed file and make sure it opened
    QFile file(_filename);

    if(!file.open(QIODevice::ReadOnly)){
        return(errno);
    }

    quint64 fileSize = QFileInfo(file).size();

    //make sure there is enough memory
    if(fileSize > sizeof(memory) - 0x200){
        return(INSUFFICIENT_MEMORY);
    }

    //read the memory and make sure it was properly stored
    QByteArray romData = file.readAll();

    if(romData.size() != fileSize){
        return(WRITE_MEM_INEQUAL);
    }

    //copy ROM into backup for reloading
    memcpy(memory + 0x200, romData.constData(), sizeof(memory) - 0x200);
    memcpy(initMemory, memory, sizeof(memory));

    //close the file and return succesful
    file.close();
    return(0);
}

void Chip8Emu::setHz(unsigned int _hz)
{
    emulatorHz = 1000000/_hz; //microseconds
}

void Chip8Emu::start()
{
    //make sure this instance isnt running
    this->quit();

    //start the threads and run
    running = true;
    paused = false;
    emulatorThread = std::thread(&Chip8Emu::emulate, this);
    timerThread = std::thread(&Chip8Emu::updateTimers, this);
}

void Chip8Emu::pause(bool _paused)
{
    //toggle pause
    paused = _paused;
}

void Chip8Emu::reload()
{
    //quit the emulator
    this->quit();

    //copy initial memory state into memory
    memcpy(memory, initMemory, sizeof(initMemory));

    //restart the emulator
    this->start();
}

void Chip8Emu::quit()
{
    //stop the threads
    running = false;
    paused = false;

    if(emulatorThread.joinable()){
        emulatorThread.join();
    }

    if(timerThread.joinable()){
        timerThread.join();
    }

    //reset all components
    resetComponents();
}

bool Chip8Emu::isRunning()
{
    return(running);
}

void Chip8Emu::updateKeypad(int _key, bool _value)
{
    //grab the mutex
    std::lock_guard<std::mutex> lock(mtx);

    //update the keypad
    if(_key < 16){
        keypad[_key] = _value;
    }
}

Chip8Emu::Chip8Emu(QObject* parent) : QObject(parent)
{
    //load instruction table with respective functions
    instructionTable[0x0] = &Chip8Emu::instruction0; //0
    instructionTable[0x1] = &Chip8Emu::instruction1; //1
    instructionTable[0x2] = &Chip8Emu::instruction2; //2
    instructionTable[0x3] = &Chip8Emu::instruction3; //3
    instructionTable[0x4] = &Chip8Emu::instruction4; //4
    instructionTable[0x5] = &Chip8Emu::instruction5; //5
    instructionTable[0x6] = &Chip8Emu::instruction6; //6
    instructionTable[0x7] = &Chip8Emu::instruction7; //7
    instructionTable[0x8] = &Chip8Emu::instruction8; //8
    instructionTable[0x9] = &Chip8Emu::instruction9; //9
    instructionTable[0xA] = &Chip8Emu::instructionA; //A
    instructionTable[0xB] = &Chip8Emu::instructionB; //B
    instructionTable[0xC] = &Chip8Emu::instructionC; //C
    instructionTable[0xD] = &Chip8Emu::instructionD; //D
    instructionTable[0xE] = &Chip8Emu::instructionE; //E
    instructionTable[0xF] = &Chip8Emu::instructionF; //F

    //init all arrays
    for(int i = 0; i < 64 * 32; ++i){
        display[i] = 0x0;
    }

    for(int i = 0; i < 4096; ++i){
        memory[i] = 0x0;
    }

    for(int i = 0; i < 4096; ++i){
        initMemory[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        stack[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        V[i] = 0x0;
    }

    for(int i = 0; i < 16; ++i){
        keypad[i] = false;
    }

    //load fonts into memory
    memcpy(memory, fonts, sizeof(fonts));
}
