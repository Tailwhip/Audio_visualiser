#ifndef FFTData_H
#define FFTData_H

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
 * @brief The FFTData class provides data samples for plotting in frequency domain.
 */
class FFTData : public QIODevice
{
    Q_OBJECT
public:
    explicit FFTData(QXYSeries *series, QObject *parent = nullptr);
    static const int sampleCount = 1024;
    static const int sampleRate = 44100;
protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
    double Hann_window (int i);
private:
    QXYSeries *m_series;
    QVector<QPointF> m_buffer;
};

#endif // FFTData_H
