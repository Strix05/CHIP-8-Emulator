#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QAction>
#include <QFileInfo>


//private functions
void MainWindow::saveColorsToFile(const QString &_filePath, const QColor &_onColor, const QColor &_offColor)
{
    //create a json object for each color
    QJsonObject colorsObject;
    colorsObject["offColor"] = _offColor.name();
    colorsObject["onColor"] = _onColor.name();

    //save colors to file
    QJsonDocument doc(colorsObject);
    QFile file(_filePath);
    if(file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadColorsFromFile(const QString &_filePath)
{
    //open the file
    QFile file(_filePath);
    if (file.open(QIODevice::ReadOnly)) {
        //read the data into a json doc then into a json obj
        QJsonDocument doc(QJsonDocument::fromJson(file.readAll()));
        QJsonObject colorsObject = doc.object();

        //read the colors from the json object and set them
        ui->display->pixOffColor = QColor(colorsObject["offColor"].toString());
        ui->display->pixOnColor = QColor(colorsObject["onColor"].toString());

        //close the file
        file.close();
    }
    else{
        qDebug() << "Couldn't locate settings file";
        return;
    }
}

void MainWindow::saveRomsToFile(const QString &_filePath)
{
    //open file
    QFile file(_filePath);
    if(file.open(QIODevice::WriteOnly)) {
        //convert romsArray to json doc
        QJsonDocument doc(romsArray);

        //write the doc
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadRomsFromFile(const QString &_filePath)
{
    //open file
    QFile file(_filePath);
    if(file.open(QIODevice::ReadOnly)){
        //read contents of file into doc
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

        //save contents into roms Array
        romsArray = doc.array();

        //iterate through each rom
        for (const QJsonValue &romValue : romsArray) {
            //make sure the value is an object
            if (romValue.isObject()) {
                //convert value into object
                QJsonObject romObject = romValue.toObject();

                //iterate through each key in object (only 1)
                for(const QString &key : romObject.keys()) {
                    //get the rom path from the object
                    QString romPath = romObject.value(key).toString();

                    //make sure path exists
                    if (QFileInfo::exists(romPath)) {
                        //add rom to the dropdown
                        addRomToDropdown(key, romPath);
                    }
                    else{
                        removeRomFromFile("ROMS.json", key);
                        qWarning() << "File does not exist:" << romPath;
                    }
                }
            }
        }
    }
    else{
        qWarning() << "Couldnt locate ROMs file";
        return;
    }


}

void MainWindow::addRomToDropdown(const QString &_romName, const QString &_romPath)
{
    //create a new action and remove action
    QAction* loadRomAction = new QAction();
    QAction* removeRomAction = new QAction();

    //set the text to the rom name
    loadRomAction->setText(_romName);
    removeRomAction->setText(_romName);

    //connect action to loading the roms path
    connect(loadRomAction, &QAction::triggered, [this, _romPath]()
    {
        loadRom(_romPath);
    });

    //connect remove action to deleting dropdown and file value
    connect(removeRomAction, &QAction::triggered, [this, loadRomAction, removeRomAction, _romName]()
    {
        ui->menuROMs->removeAction(loadRomAction);
        ui->menuRemove_ROM->removeAction(removeRomAction);
        removeRomFromFile("ROMS.json", _romName);
    });

    //add action and remove action to dropdowns
    ui->menuROMs->addAction(loadRomAction);
    ui->menuRemove_ROM->addAction(removeRomAction);
}

void MainWindow::removeRomFromFile(const QString &_filePath, const QString &_romName)
{
    QFile file(_filePath);

    if (!file.open(QIODevice::ReadWrite)){
        qDebug() << "Couldn't locate ROMs file";
        return;
    }

    //grab the array from the file
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    romsArray = doc.array();

    //check each element for the rom name and delete it when found
    for (int i = 0; i < romsArray.size(); ++i) {
        QJsonObject romObject = romsArray.at(i).toObject();
        if (romObject.contains(_romName)) {
            romsArray.removeAt(i);
            break;
        }
    }

    //clear the document
    file.resize(0);
    file.seek(0);

    //save updated array to file
    doc.setArray(romsArray);
    file.write(doc.toJson());

    //close the file
    file.close();
}

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("CHIP8 Emulator");

    //connect emulator to everything
    connect(&emulator, &Chip8Emu::draw_flag, this->ui->display, &EmuDisplayWidget::draw);
    // connect(&emulator, &Chip8Emu::error, [this]() {})
    connect(this, &MainWindow::keyChange, &emulator, &Chip8Emu::updateKeypad);
    connect(this, &MainWindow::pauseKey, &emulator, &Chip8Emu::pause);
    connect(this, &MainWindow::quitKey, &emulator, &Chip8Emu::quit);
    connect(this, &MainWindow::reloadKey, &emulator, &Chip8Emu::reload);

    //load settings
    loadColorsFromFile("settings.json");
    loadRomsFromFile("ROMS.json");

    //temp
    emulator.setHz(800);
}

MainWindow::~MainWindow()
{
    emulator.quit();
    delete ui;
}

//protected functions
void MainWindow::keyPressEvent(QKeyEvent *_event)
{
    switch(_event->key()){

    case Qt::Key_X:
        emit keyChange(0, true);
        return;

    case Qt::Key_1:
        emit keyChange(1, true);
        return;

    case Qt::Key_2:
        emit keyChange(2, true);
        return;

    case Qt::Key_3:
        emit keyChange(3, true);
        return;

    case Qt::Key_Q:
        emit keyChange(4, true);
        return;

    case Qt::Key_W:
        emit keyChange(5, true);
        return;

    case Qt::Key_E:
        emit keyChange(6, true);
        return;

    case Qt::Key_A:
        emit keyChange(7, true);
        return;

    case Qt::Key_S:
        emit keyChange(8, true);
        return;

    case Qt::Key_D:
        emit keyChange(9, true);
        return;

    case Qt::Key_Z:
        emit keyChange(0xA, true);
        return;

    case Qt::Key_C:
        emit keyChange(0xB, true);
        return;

    case Qt::Key_4:
        emit keyChange(0xC, true);
        return;

    case Qt::Key_R:
        emit keyChange(0xD, true);
        return;

    case Qt::Key_F:
        emit keyChange(0xE, true);
        return;

    case Qt::Key_V:
        emit keyChange(0xF, true);
        return;

    case Qt::Key_Escape:
        emit quitKey();
        qApp->quit();
        return;

    case Qt::Key_Space:
        paused = !paused;
        emit pauseKey(paused);
        return;

    case Qt::Key_0:
        emit reloadKey();
        return;
    }

}

void MainWindow::keyReleaseEvent(QKeyEvent *_event)
{
    switch(_event->key()){

    case Qt::Key_X:
        emit keyChange(0, false);
        return;

    case Qt::Key_1:
        emit keyChange(1, false);
        return;

    case Qt::Key_2:
        emit keyChange(2, false);
        return;

    case Qt::Key_3:
        emit keyChange(3, false);
        return;

    case Qt::Key_Q:
        emit keyChange(4, false);
        return;

    case Qt::Key_W:
        emit keyChange(5, false);
        return;

    case Qt::Key_E:
        emit keyChange(6, false);
        return;

    case Qt::Key_A:
        emit keyChange(7, false);
        return;

    case Qt::Key_S:
        emit keyChange(8, false);
        return;

    case Qt::Key_D:
        emit keyChange(9, false);
        return;

    case Qt::Key_Z:
        emit keyChange(0xA, false);
        return;

    case Qt::Key_C:
        emit keyChange(0xB, false);
        return;

    case Qt::Key_4:
        emit keyChange(0xC, false);
        return;

    case Qt::Key_R:
        emit keyChange(0xD, false);
        return;

    case Qt::Key_F:
        emit keyChange(0xE, false);
        return;

    case Qt::Key_V:
        emit keyChange(0xF, false);
        return;
    }
}

//private slots
void MainWindow::on_actionPixel_On_triggered()
{
    //paused the emulator
    paused = true;
    emulator.pause(true);

    //change the color
    QColor newColor = QColorDialog::getColor();
    ui->display->pixOnColor = newColor;

    //save to file
    saveColorsToFile("settings.json", newColor, ui->display->pixOffColor);
}

void MainWindow::on_actionPixel_Off_triggered()
{
    //pause the emulator
    paused = true;
    emulator.pause(true);

    //change the color
    QColor newColor = QColorDialog::getColor();
    ui->display->pixOffColor = newColor;

    //save to file
    saveColorsToFile("settings.json", ui->display->pixOnColor, newColor);
}

void MainWindow::on_actionLoad_New_ROM_triggered()
{
    //quit the emulator
    emulator.quit();

    //create file window
    QFileDialog fileDialog;

    //make sure file exists
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setViewMode(QFileDialog::Detail);

    //get the file path and make sure its a .ch8
    QString filePath = fileDialog.getOpenFileName(this, tr("Open ROM File"), "", tr("ROM Files (*.ch8)"));

    //prompt user for ROM name
    QString fileName = QInputDialog::getText(nullptr, "Enter ROM Name", "Name:");

    //create json object with rom name and path
    QJsonObject newRom;
    newRom[fileName] = filePath;

    //append rom to array
    romsArray.append(newRom);

    //add it to the dropdown
    addRomToDropdown(fileName, filePath);

    //save roms to file
    saveRomsToFile("ROMS.json");

    //load the rom
    emulator.loadROM(filePath);

    //start the emulator unpaused
    emulator.start();
    paused = false;
    emulator.pause(false);
}

void MainWindow::loadRom(const QString &_romPath)
{
    //quit the emulator
    emulator.quit();

    //load the rom
    if(emulator.loadROM(_romPath)){
        qDebug() << "ROM not found";
    }
    else{
        //start the emulator unpaused
        emulator.start();
        paused = false;
        emulator.pause(false);
    }
}

