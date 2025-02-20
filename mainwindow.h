#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

#include <typedefs.h>
#include <chip8emu.h>
#include <emudisplaywidget.h>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;

    //emulator
    Chip8Emu emulator;
    bool paused = false;
    QJsonArray romsArray;

    //saving and laoding settings
    void saveColorsToFile(const QString& _filePath, const QColor& _onColor, const QColor& _offColor);
    void loadColorsFromFile(const QString& _filePath);

    void saveRomsToFile(const QString& _filePath);
    void loadRomsFromFile(const QString& _filePath);
    void addRomToDropdown(const QString& _romName, const QString& _romPath);
    void removeRomFromFile(const QString& _filePath, const QString& _romName);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* _event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;

private slots:
    void on_actionPixel_On_triggered();
    void on_actionPixel_Off_triggered();
    void on_actionLoad_New_ROM_triggered();
    void loadRom(const QString& _romPath);

public slots:

signals:
    void keyChange(int _key, bool _value);
    void pauseKey(bool _paused);
    void quitKey();
    void reloadKey();
};
#endif // MAINWINDOW_H
