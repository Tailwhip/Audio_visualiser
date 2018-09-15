#include "audiodata.h"
#include <QtCharts/QXYSeries>
#include <fftw3.h>
#include <math.h>
#include <iostream>

#define LOG(x) qDebug("%s:%d: %s", __FUNCTION__, __LINE__, x)
#define REAL 0
#define IMAG 1

/**
 * @brief audioData::audioData is the constructor of class audioData.
 * @param series are audio data samples collected from the outer device (microphone) converted to a form usefull for plotting.
 * @param parent returns a pointer to the parent object.
 */
AudioData::AudioData(QXYSeries *series, QObject *parent, bool d_type) :
    QIODevice(parent),
    dataSeries(series)
{
    dataType=d_type;
}

/**
 * @brief audioData::readData is a virtual function inherited from IODevice class. Unsused in this class.
 */
qint64 AudioData::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}

/**
 * @brief FFTData::writeData is a virtual function inherited from IODevice class. It writes up to maxSize bytes from data to
 * the m_series buffer. Collects audio data and make them usefull for plotting in time domain.
 * @param data contains collected audio samples.
 * @param maxSize is equal to maximum bytes size of collected data.
 */
qint64 AudioData::writeData(const char *data, qint64 maxSize)
{
    if (dataType == 1){
        return fftData(f_resolution, sampleRate, f_sampleCount, data, maxSize);
    }
    else
        return timeData(t_resolution, t_sampleCount, data, maxSize);
}

/**
 * @brief AudioData::timeData prepares collected data for plotting audio samples in time domain.
 * @param resolution is responsible for ammount of audio data showed in plot.
 * @param sampleCount provides number of samples that are going to be delivered for plotting.
 * @param audioData is a bunch of audio samples.
 * @param maximumSize returns a number of maximum audio samples collected from outter device.
 */
qint64 AudioData::timeData(int resolution, int sampleCount, const char *&audioData, qint64 &maximumSize)
{
    createBuffer(sampleCount);
    int start = 0;
    const int availableSamples = int(maximumSize) / resolution;
    if (availableSamples < sampleCount){
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            buffer[s].setY(buffer.at(s + availableSamples).y());
    }

    for (int s = start; s < sampleCount; ++s, audioData += resolution)
        buffer[s].setY(qreal(uchar(*audioData)-128) / qreal(128));

    dataSeries->replace(buffer);
    return (sampleCount - start) * resolution;
}

/**
 * @brief AudioData::fftData prepares collected data for plotting audio samples in frequency domain - transform data using FFT.
 * @param resolution is responsible for ammount of audio data showed in plot.
 * @param sampleRate provides number of samples that are going to be delivered for plotting.
 * @param sampleCount provides number of samples that are going to be delivered for plotting.
 * @param audioData is a bunch of audio samples.
 * @param maximumSize returns a number of maximum audio samples collected from outter device.
 */
qint64 AudioData::fftData(int resolution, int sampleRate, int sampleCount, const char *&audioData, qint64 &maximumSize)
{
    if (maximumSize < 800) {
        return maximumSize;
    }
    sampleCount = maximumSize;
    qDebug("maximumSize = %d", maximumSize);
    qDebug("maximumSize = %d", maximumSize);

    QVector<qreal> fftBuffor;
    //adding harvested samples into buffer:
    for (int s = 0; s < maximumSize; ++s, ++audioData){
        qreal temp = qreal(uchar(*audioData));
        fftBuffor.push_back(temp);
    }

    //making the complex plan in and out:
    fftw_complex in[maximumSize];
    fftw_complex out[maximumSize];
    for (int i = 0; i < maximumSize; ++i){
        //adding a window function and data to the complex plan:
        in[i][REAL] = fftBuffor[i] * hannWindow(i, maximumSize);
        in[i][IMAG] = 0;
//        qDebug("in[%d]REAL: %d, fftBuffor[%d]: %d", i, in[i][REAL], i, fftBuffor[i]);
    }


    //Fourier transformation:
    fftw_plan plan = fftw_plan_dft_1d(maximumSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    //setting data for DC frequency:
    QVector<QPointF> localBuffer(0);
    QPointF firstPoint;
    firstPoint.setY(log10(((out[0][REAL] * out[0][REAL]) + (out[0][IMAG] * out[0][IMAG]))));
    firstPoint.setX(sampleRate / maximumSize);
    localBuffer.push_back(firstPoint);

    /* since only half of computed fft data is needed (rest is mirrored)
       a frequency domain length is beeing setted: */
    int halfBuffer = (maximumSize / 2);

    //putting computed data into buffer for the rest of frequency domain:
    for (int i = 1; i < halfBuffer; ++i){
        //compute magnitude [dB]:
        double mag = sqrt((out[i][REAL] * out[i][REAL]) + (out[i][IMAG] * out[i][IMAG]));
        double magnitude = 20 * log10(mag);
        magnitude = magnitude * magnitude * magnitude;
        magnitude = magnitude / 10000;
        localBuffer.push_back(QPointF(i * (sampleRate / maximumSize), magnitude));
//        qDebug("creating i: %d x: %d y: %d", i, i * (sampleRate / maximumSize), magnitude);
    }

    for(int i = 0; i < localBuffer.size(); ++i) {
        qDebug("i: %f x: %f y: %f", i, localBuffer[i].x(), localBuffer[i].y());
    }

    //taking results out of function:
    //omijanie rysowanie
    dataSeries->replace(localBuffer);
    return maximumSize;
}

/**
 * @brief AudioData::createBuffer provides a zeros filled buffer for audi data.
 * @param sampleCount is an argument using to decide if a new buffer is needed.
 */
void AudioData::createBuffer(int sampleCount)
{
    if (buffer.isEmpty()){
        buffer.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            buffer.append(QPointF(i, 0));
    }
}

/**
 * @brief AudioData::hannWindow is a function that transforms collected audio data with Hanning Window function.
 * @param iterator is a number of collected data in buffer to transform.
 * @param sampleCount is a number of all samples to transform.
 */
double AudioData::hannWindow(int iterator, int sampleCount)
{
    return 0.5 * (1 - cos(2 * M_PI * iterator / (sampleCount - 1)));
}

/**
 * @brief AudioData::~AudioData is an inherited destructor of an AudioData class. Cleans up FFT plan.
 */
AudioData::~AudioData()
{
    //cleaning up a plan:
    fftw_destroy_plan(plan);
    fftw_cleanup();
}
