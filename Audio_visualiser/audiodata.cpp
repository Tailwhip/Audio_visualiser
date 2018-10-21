#include "audiodata.h"
#include <QtCharts/QXYSeries>
#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <QTimerEvent>
#include <QTimer>

#define REAL 0
#define IMAG 1

/**
 * @brief audioData::audioData is the constructor of class audioData.
 * @param series are audio data samples collected from the outer device (microphone) converted to a form usefull for plotting.
 * @param parent returns a pointer to the parent object.
 * @param d_type is a flag deciding which datas (time or FFT) will be returned. 1 returns FFT data, 0 returns
 * time data.
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
    if (dataType == 1) {
        return fftData(sampleRate, data, maxSize);
    }
    else
        return timeData(t_sampleCount, data, maxSize);
}

/**
 * @brief AudioData::timeData prepares collected data for plotting audio samples in time domain.
 * @param sampleCount provides number of samples that are going to be delivered for plotting.
 * @param audioData is a bunch of audio samples.
 * @param maximumSize returns a number of maximum audio samples collected from outter device.
 */
qint64 AudioData::timeData(int sampleCount, const char *&audioData, qint64 &maximumSize)
{
    //creating a buffer if it's needed
    int resolution = 2;
    if (buffer.isEmpty()) {
        buffer.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            buffer.append(QPointF(i, 0));
    }

    /*not all bunches of samples have the same length so they needs to be groupped to a
    samplecount bunches
    */
    int start = 0;
    const int availableSamples = int(maximumSize) / resolution;
    if (availableSamples < sampleCount){
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            buffer[s].setY(buffer.at(s + availableSamples).y());
    }

    //scalling collected data
    for (int s = start; s < sampleCount; ++s, audioData += resolution)
        buffer[s].setY(qreal(uchar(*audioData)-128) / qreal(128));

    //taking results out of function
    dataSeries->replace(buffer);
    return (sampleCount - start) * resolution;
}

/**
 * @brief AudioData::fftData prepares collected data for plotting audio samples in frequency domain - transform data using FFT.
 * @param sampleRate provides number of samples that are going to be delivered for plotting.
 * @param sampleCount provides number of samples that are going to be delivered for plotting.
 * @param audioData is a bunch of audio samples.
 */
qint64 AudioData::fftData(int sampleRate, const char *&audioData, qint64 &sampleCount)
{
    //dont't show bunch of samples less than specified number
    if (sampleCount < 1024) {
        return sampleCount;
    }

    //adding harvested samples into buffer
    QVector<qreal> fftBuffer;
    for (int s = 0; s < sampleCount; ++s, ++audioData) {
        qreal temp = qreal(uchar(*audioData));
        fftBuffer.push_back(temp);
    }

    //making the complex plan in and out
    fftw_complex in[sampleCount];
    fftw_complex out[sampleCount];
    for (int i = 0; i < sampleCount; ++i) {
        //adding a window function and data to the complex plan
        in[i][REAL] = fftBuffer[i] * hannWindow(i, int(sampleCount));
        in[i][IMAG] = 0;
    }

    //Fourier transformation
    fftw_plan plan = fftw_plan_dft_1d(int(sampleCount), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    //setting data for DC frequency
    int resolution = sampleRate / sampleCount;
    QVector<QPointF> localBuffer(0);
    QPointF DCPoint;
    DCPoint.setY(log10((out[0][REAL] * out[0][REAL]) + (out[0][IMAG] * out[0][IMAG])) / 100);
    DCPoint.setX(0);
    localBuffer.push_back(DCPoint);

    /* since only half of computed fft data is needed (rest is mirrored)
       a frequency domain length is beeing setted: */
    int halfBuffer = (int(sampleCount) / 2);
    //putting computed data into buffer for the rest of frequency domain
    for (int i = 1; i < halfBuffer; ++i) {
        //compute magnitude [dB]
        double magnitude = 10 * log10((2*(out[i][REAL] * out[i][REAL]) + (out[i][IMAG] * out[i][IMAG])) / 100);
        magnitude = filter(magnitude);
        localBuffer.push_back(QPoint(i * resolution, int(magnitude)));
    }

    //taking results out of function
    maxFreq = charFreq(localBuffer);
    dataSeries->replace(localBuffer);
    return sampleCount;
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
 * @brief AudioData::filter function makes fft magnitude easer to read on plot. It reduces a noise and powers
 * the proper signal.
 * @param magnitude provides to a function a magnitude to be filterred.
 */
double AudioData::filter(double magnitude)
{
    magnitude = (magnitude * magnitude * magnitude) / 8000;
    if (magnitude < 3)
            magnitude = 0;
    return magnitude;
}

/**
 * @brief AudioData::charFreq provides frequency of the maximum value of fftdata.
 * @param buffer is the bunch of values search for maximum frequency.
 */
double AudioData::charFreq(QVector<QPointF> &buffer)
{
    double max = 0;
    double freq = 0;
    if (dataType == 1 && !buffer.isEmpty()) {
        for (int i = 30; i < buffer.size(); ++i) {
            if (double (buffer[i].y()) > max) {
                max = buffer[i].y();
                freq = buffer[i].x();
            }
        }
    }
    return freq;
}

/**
 * @brief AudioData::~AudioData is an inherited destructor of an AudioData class. Cleans up FFT plan.
 */
AudioData::~AudioData()
{
    fftw_destroy_plan(plan);
    fftw_cleanup();
}
