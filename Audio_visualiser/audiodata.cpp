#include "audiodata.h"
#include <QtCharts/QXYSeries>
#include <fftw3.h>
#include <math.h>
#include <iostream>

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
    //creating a buffer:
    createBuffer(sampleCount);

    int start = 0;
    const int availableSamples = int(maximumSize) / resolution;
    if (availableSamples < sampleCount){
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            buffer[s].setY(buffer.at(s + availableSamples).y());
    }

    //adding harvested samples to buffer:
    for (int s = start; s < sampleCount; ++s, audioData += resolution){
        buffer[s].setY(qreal(uchar(*audioData)-128) / qreal(128));
    }

    //making complex plan:
    fftw_complex in[sampleCount];
    fftw_complex out[sampleCount];
    for (int i = 0; i < sampleCount; ++i){
        //adding a window function and data to complex plan:
        in[i][REAL] = buffer[i].y() * hannWindow(i, sampleCount);
        in[i][IMAG] = 0;
    }
    //Fourier transformation:
    fftw_plan plan = fftw_plan_dft_1d(sampleCount, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    /* since only half of computed fft data is needed (rest is mirrored)
       a proper costants is beeing setted: */
    int halfBuffer = (sampleCount / 2);
    double dB_magnitude[halfBuffer];
    //putting computed data into buffer:
    for (int i = 0; i < halfBuffer; ++i){
        //compute magnitude [dB]:
        dB_magnitude[i] = 10 * log10(((out[i][REAL] * out[i][REAL]) + (out[i][IMAG] * out[i][IMAG])));
        //setting amplitude:
        buffer[i].setY(dB_magnitude[i]);
        //setting frequency domain:
        buffer[i].setX(i * (sampleRate / sampleCount));
    }

    //taking results out of function:
    dataSeries->replace(buffer);
    return (sampleCount - start) * resolution;
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
