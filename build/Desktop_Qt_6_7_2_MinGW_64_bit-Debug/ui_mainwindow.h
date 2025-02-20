/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>
#include "emudisplaywidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionHz;
    QAction *actionPixel_On;
    QAction *actionPixel_Off;
    QAction *actionControls;
    QAction *actionLoad_New_ROM;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    EmuDisplayWidget *display;
    QMenuBar *menubar;
    QMenu *menuSettings;
    QMenu *menuColors_2;
    QMenu *menuRemove_ROM;
    QMenu *menuROMs;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1536, 789);
        MainWindow->setMinimumSize(QSize(1536, 789));
        MainWindow->setMaximumSize(QSize(1536, 789));
        MainWindow->setAcceptDrops(true);
        actionHz = new QAction(MainWindow);
        actionHz->setObjectName("actionHz");
        actionPixel_On = new QAction(MainWindow);
        actionPixel_On->setObjectName("actionPixel_On");
        actionPixel_Off = new QAction(MainWindow);
        actionPixel_Off->setObjectName("actionPixel_Off");
        actionControls = new QAction(MainWindow);
        actionControls->setObjectName("actionControls");
        actionLoad_New_ROM = new QAction(MainWindow);
        actionLoad_New_ROM->setObjectName("actionLoad_New_ROM");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setMinimumSize(QSize(1536, 768));
        centralwidget->setMaximumSize(QSize(1536, 768));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        display = new EmuDisplayWidget(centralwidget);
        display->setObjectName("display");

        horizontalLayout->addWidget(display);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1536, 21));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName("menuSettings");
        menuColors_2 = new QMenu(menuSettings);
        menuColors_2->setObjectName("menuColors_2");
        menuRemove_ROM = new QMenu(menuSettings);
        menuRemove_ROM->setObjectName("menuRemove_ROM");
        menuROMs = new QMenu(menubar);
        menuROMs->setObjectName("menuROMs");
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuSettings->menuAction());
        menubar->addAction(menuROMs->menuAction());
        menuSettings->addAction(actionLoad_New_ROM);
        menuSettings->addAction(menuRemove_ROM->menuAction());
        menuSettings->addAction(menuColors_2->menuAction());
        menuColors_2->addAction(actionPixel_On);
        menuColors_2->addAction(actionPixel_Off);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionHz->setText(QCoreApplication::translate("MainWindow", "Emulator Settings", nullptr));
        actionPixel_On->setText(QCoreApplication::translate("MainWindow", "Pixel On", nullptr));
        actionPixel_Off->setText(QCoreApplication::translate("MainWindow", "Pixel Off", nullptr));
        actionControls->setText(QCoreApplication::translate("MainWindow", "Controls", nullptr));
        actionLoad_New_ROM->setText(QCoreApplication::translate("MainWindow", "Load New ROM", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        menuColors_2->setTitle(QCoreApplication::translate("MainWindow", "Colors", nullptr));
        menuRemove_ROM->setTitle(QCoreApplication::translate("MainWindow", "Remove ROM", nullptr));
        menuROMs->setTitle(QCoreApplication::translate("MainWindow", "ROMs", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
