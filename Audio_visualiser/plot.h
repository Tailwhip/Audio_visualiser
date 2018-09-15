#ifndef PLOT_H
#define PLOT_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtMultimedia/QAudioInput>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE
class AudioData;

QT_BEGIN_NAMESPACE
class QAudioInput;
class QAudioDeviceInfo;
QT_END_NAMESPACE
/**
 * @brief The Plot class provides plots for time and frequency domain of audio data.
 */
class Plot : public QWidget
{
    Q_OBJECT

public:
    const QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultInputDevice();
    explicit Plot(QWidget *parent = nullptr);
    void showFreq();
    void showTime();
    void setPlot();
    void pause();
    void start();
    void clear();
    ~Plot();

private:
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QAudioFormat audioFormat;
    AudioData *device = nullptr;
    QChart *chart;
    QLineSeries *series;
    QAudioInput *audioInput = nullptr;
    QBrush brush;
    bool setPlotFlag = 0;
};

#endif // PLOT_H
