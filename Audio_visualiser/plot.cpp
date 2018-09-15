#include "plot.h"
#include "audiodata.h"
#include "mainwindow.h"

#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>

#include <math.h>


QT_CHARTS_USE_NAMESPACE
/**
 * @brief Plot::Plot is the constructor of Plot class.
 * @param parent returns a pointer to the parent object.
 */
Plot::Plot(QWidget *parent) :
    QWidget(parent),
    chart(new QChart),
    series(new QLineSeries)
{
    Plot::setPlot();
}
/**
 * @brief Plot::setPlot function provides all necessary configurations to set a plot.
 */
void Plot::setPlot()
{
    QChartView *chartView = new QChartView(chart);
    chartView->setMinimumSize(400, 300);
    chart->addSeries(series);
    //plot colour:
    QPen pen(QRgb(0x2e004d));
    pen.setWidth(1);
    series->setPen(pen);

    QFont font;
    font.setPixelSize(18);
    brush.setColor(QRgb(0xa6a6a6));
    chart->setTitleFont(font);
    chart->setTitleBrush(brush);

    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0x1f0033));
    backgroundGradient.setColorAt(1.0, QRgb(0x1a1a1a));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(backgroundGradient);

    QLinearGradient plotAreaGradient;
    plotAreaGradient.setStart(QPointF(0, 1));
    plotAreaGradient.setFinalStop(QPointF(1, 0));
    plotAreaGradient.setColorAt(0.0, QRgb(0xa6a6a6));
    plotAreaGradient.setColorAt(1.0, QRgb(0x737373));
    plotAreaGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setPlotAreaBackgroundBrush(plotAreaGradient);
    chart->setPlotAreaBackgroundVisible(true);

    mainLayout->addWidget(chartView);

    axisX->setRange(0, 999);
    axisX->setTitleText("None");
    axisX->setLabelFormat("%g");
    axisX->setLabelsColor(QRgb(0xa6a6a6));
    axisX->setTitleBrush(brush);

    axisY->setRange(-1, 1);
    axisY->setLabelsColor(QRgb(0xa6a6a6));
    axisY->setTitleBrush(brush);
    axisY->setTitleText("None");

    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    chart->legend()->hide();
    chart->setTitle(" ");

    setPlotFlag=1;
}
/**
 * @brief Plot::showFreq function shows a plot of audio data in frequency domain.
 */
void Plot::showFreq()
{ 
    if (setPlotFlag==0)
    {
        QMessageBox::warning(nullptr, "audio", "Set the plot first!");
        exit(EXIT_FAILURE);
    }

    //setting the device to obtain audio data transformed with FFT
    device = new AudioData(series, this, 1);

    audioFormat.setSampleRate(device->sampleRate);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(8);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::UnSignedInt);

    axisX->setRange(0, 15000);
    axisX->setTitleText("Frequency [Hz]");
    axisY->setRange(0, 80);
    axisY->setTitleText("Audio level [dB]");

    chart->setTitle("Audio spectrum");

    audioInput = new QAudioInput(deviceInfo, audioFormat, this);

    device->open(QIODevice::WriteOnly);
    audioInput->start(device);
}

/**
 * @brief Plot::showTime function shows a plot of audio data in time domain.
 */

void Plot::showTime()
{
    if (setPlotFlag==0)
    {
        QMessageBox::warning(nullptr, "audio", "Set the plot first!");
        exit(EXIT_FAILURE);
    }

    //setting the device to obtain audio data in time domain
    device = new AudioData(series, this, 0);

    audioFormat.setSampleRate(device->sampleRate);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(8);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::UnSignedInt);

    axisX->setRange(0, device->t_sampleCount);
    axisX->setTitleText("Time [s]");
    axisY->setRange(-1, 1);
    axisY->setTitleText("Audio level");

    chart->setTitle("Audio level vs. time");

    audioInput = new QAudioInput(deviceInfo, audioFormat, this);
    device->open(QIODevice::WriteOnly);
    audioInput->start(device);
}
/**
 * @brief Plot::pause function pauses plots.
 */
void Plot::pause()
{
    audioInput->suspend();
}
/**
 * @brief Plot::start function starts plots.
 */
void Plot::start()
{
    audioInput->start(device);
}
/**
 * @brief Plot::clear function clears plots.
 */
void Plot::clear()
{
    audioInput->stop();
    series->clear();
}
/**
 * @brief Plot::~Plot is a destructor of a Plot object. It releases all dynamically allocated objects and variables.
 * Also cleans up plots.
 */
Plot::~Plot()
{
    audioInput->stop();
    series->clear();
    device->close();
    delete chart;
    delete device;
    delete audioInput;
    delete axisX;
    delete axisY;
    delete mainLayout;
}
