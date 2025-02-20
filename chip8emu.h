#ifndef CHIP8EMU_H
#define CHIP8EMU_H

#include <typedefs.h>
#include <QObject>
#include <mutex>
#include <thread>

class Chip8Emu : public QObject
{
    Q_OBJECT
private:
    //emulator parameters
    typedef struct{
        bool verbose;
        bool verboseReg;
        bool originalInstruction8;
        bool originalInstructionB;
        bool instructionIncrementF;
        bool instructionOverflowF;
    }Params;

    //emulator fonts
    u8 fonts[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    //emulator architecture
    bool keypad[16];
    u8 display[64 * 32];
    u8 memory[4096];
    u8 initMemory[4096];
    u16 stack[16];
    u8 V[16];
    u16 PC = 0x200;
    u8 SP = 0;
    u16 I = 0;
    u8 delay_timer = 0;
    u8 sound_timer = 0;

    //runtime
    int timerHz = 1000/60;
    int emulatorHz = 1000000/700;
    bool running = false;
    bool paused = false;
    std::thread emulatorThread;
    std::thread timerThread;
    std::mutex mtx;

    //emulator functions
    void printInstruction(char* _message);
    void pushStack(u16 _push);
    void popStack(u16* _pop);
    void emulate(void);
    void updateTimers(void);
    void resetComponents(void);

    //instruction
    u16 instruction = 0;
    u8 x = 0;
    u8 y = 0;

    //instruction lookup table
    void (Chip8Emu::*instructionTable[16])(void);

    //instruction table functions
    void instruction0(void); //multiple
    void instruction1(void);
    void instruction2(void);
    void instruction3(void);
    void instruction4(void);
    void instruction5(void);
    void instruction6(void);
    void instruction7(void);
    void instruction8(void); //multiple
    void instruction9(void);
    void instructionA(void);
    void instructionB(void);
    void instructionC(void);
    void instructionD(void);
    void instructionE(void); //multiple
    void instructionF(void); //multiple

public:
    //error codes
    typedef enum{
        //load ROM
        FILE_NOT_FOUND = 2,
        WRITE_MEM_INEQUAL = 3,
        INSUFFICIENT_MEMORY = 12,
        FILE_PERMISSIONS = 13,
        DIR_NOT_FILE = 21,
        FILE_NAME_TOO_LONG = 36,

        //emulator
        STACK_FULL = 40,
        STACK_EMPTY = 41,
        UNKNOWN_INSTRUCTION = 42,
        ROM_NOT_LOADED = 43
    }ErrorCodes;

    //debug
    Params params {false, false, false, false, false, false};

    //functions
    int loadROM(const QString _filename);
    void setHz(unsigned int _hz);
    bool isRunning();

    //constructor
    explicit Chip8Emu(QObject* parent = nullptr);

public slots:
    void updateKeypad(int _key, bool _value);
    void start();
    void pause(bool _paused);
    void reload();
    void quit();

signals:
    void draw_flag(u8* _frame);
    void sound_flag();
    void error(int _error);
};

#endif // CHIP8EMU_H
