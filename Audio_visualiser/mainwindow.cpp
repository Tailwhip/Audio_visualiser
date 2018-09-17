#include "mainwindow.h"
#include "plot.h"
#include "audiodata.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QTimerEvent>
#include <QTimer>
#include <iostream>
/**
 * @brief MainWindow::MainWindow is a constructor of a MainWindow class.
 * @param parent returns a pointer to the parent object.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

/**
 * @brief MainWindow::~MainWindow is a destructor of a MainWindow class.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_ButtonStart_clicked function shows two plots of audio data - in time and in frequency domain.
 */
void MainWindow::on_ButtonStart_clicked()
{

    if (deviceInfo.isNull()){
        QMessageBox::warning(nullptr, "audio", "There is no audio input device available.");
        exit(EXIT_FAILURE);
    }
    ui->Plot1->showFreq();
    ui->Plot2->showTime();
    ui->ButtonPause->setEnabled(true);
    ui->ButtonClear->setEnabled(true);
    ui->ButtonStart->setEnabled(false);
    QTimer *timer = ui->Plot1->device->timer;
    timer->start(200);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
}

/**
 * @brief MainWindow::on_ButtonPause_clicked function pauses two plots of audio data - in time and in frequency domain.
 */
void MainWindow::on_ButtonPause_clicked()
{
    if (ui->ButtonPause->text()=="PAUSE"){
        ui->ButtonPause->setText("CONTINUE");
        ui->Plot1->pause();
        ui->Plot2->pause();
    }
    else {
        ui->ButtonPause->setText("PAUSE");
        ui->Plot1->start();
        ui->Plot2->start();
    }
}

/**
 * @brief MainWindow::on_ButtonClear_clicked function clears two plots of audio data - in time and in frequency domain.
 */
void MainWindow::on_ButtonClear_clicked()
{
    ui->Plot1->clear();
    ui->Plot2->clear();
    ui->ButtonPause->setText("CONTINUE");
}

void MainWindow::timerUpdate()
{
    value = ui->Plot1->device->maxFreq;
    ui->lcdNumber->display(value);
}
