/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    Plot *Plot1;
    Plot *Plot2;
    QPushButton *ButtonStart;
    QPushButton *ButtonPause;
    QPushButton *ButtonClear;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(959, 689);
        MainWindow->setWindowOpacity(1);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QStringLiteral("background-color: rgb(26, 26, 26);"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Plot1 = new Plot(centralWidget);
        Plot1->setObjectName(QStringLiteral("Plot1"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Plot1->sizePolicy().hasHeightForWidth());
        Plot1->setSizePolicy(sizePolicy);
        Plot1->setContextMenuPolicy(Qt::DefaultContextMenu);
        Plot1->setAcceptDrops(false);
        Plot1->setLayoutDirection(Qt::LeftToRight);
        Plot1->setAutoFillBackground(false);
        Plot1->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(Plot1, 0, 0, 1, 3);

        Plot2 = new Plot(centralWidget);
        Plot2->setObjectName(QStringLiteral("Plot2"));
        sizePolicy.setHeightForWidth(Plot2->sizePolicy().hasHeightForWidth());
        Plot2->setSizePolicy(sizePolicy);

        gridLayout->addWidget(Plot2, 1, 0, 1, 3);

        ButtonStart = new QPushButton(centralWidget);
        ButtonStart->setObjectName(QStringLiteral("ButtonStart"));
        ButtonStart->setEnabled(true);
        QPalette palette;
        QBrush brush(QColor(31, 0, 51, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        ButtonStart->setPalette(palette);
        QFont font;
        font.setFamily(QStringLiteral("Calibri"));
        font.setPointSize(26);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        ButtonStart->setFont(font);
        ButtonStart->setStyleSheet(QStringLiteral("background-color: rgb(31, 0, 51);"));

        gridLayout->addWidget(ButtonStart, 2, 0, 1, 1);

        ButtonPause = new QPushButton(centralWidget);
        ButtonPause->setObjectName(QStringLiteral("ButtonPause"));
        ButtonPause->setEnabled(false);
        ButtonPause->setFont(font);
        ButtonPause->setAutoFillBackground(false);
        ButtonPause->setStyleSheet(QStringLiteral("background-color: rgb(31, 0, 51);"));

        gridLayout->addWidget(ButtonPause, 2, 1, 1, 1);

        ButtonClear = new QPushButton(centralWidget);
        ButtonClear->setObjectName(QStringLiteral("ButtonClear"));
        ButtonClear->setEnabled(false);
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Button, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush);
        ButtonClear->setPalette(palette1);
        ButtonClear->setFont(font);
        ButtonClear->setStyleSheet(QStringLiteral("background-color: rgb(31, 0, 51);"));

        gridLayout->addWidget(ButtonClear, 2, 2, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 959, 20));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setEnabled(false);
        mainToolBar->setMovable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Audio Visualiser", nullptr));
        ButtonStart->setText(QApplication::translate("MainWindow", "START", nullptr));
        ButtonPause->setText(QApplication::translate("MainWindow", "PAUSE", nullptr));
        ButtonClear->setText(QApplication::translate("MainWindow", "CLEAR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
