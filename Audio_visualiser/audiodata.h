#ifndef audioData_H
#define audioData_H

#include <QtCore/QIODevice>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtCharts/QChartGlobal>
#include <fftw3.h>

QT_CHARTS_BEGIN_NAMESPACE
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE
/**
 * @brief The audioData class provides data samples for plotting in time domain.
 */
class AudioData : public QIODevice
{
    Q_OBJECT
public:
    ~AudioData() override;
    qint64 timeData(int resolution, int sampleCount, const char *&audioData, qint64 &maximumSize);
    qint64 fftData(int resolution, int sampleRate, int sampleCount, const char *&audioData, qint64 &maximumSize);
    explicit AudioData(QXYSeries *series, QObject *parent = nullptr, bool d_type = 0);
    static const int t_resolution = 8;
    static const int t_sampleCount = 8000;
    static const int f_resolution = 2;
    static const int f_sampleCount = 1024;
    static const int sampleRate = 44100;
protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
    double hannWindow (int i, int sampleCount);
private:
    bool dataType = 0;
    void createBuffer(int sampleCount);
    QXYSeries *dataSeries;
    QVector<QPointF> buffer;
    fftw_plan plan = nullptr;
};

#endif // audioData_H
