#include <QApplication>
#include <mainwindow.h>

int main(int argc, char** argv)
{
    //Qt application
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
