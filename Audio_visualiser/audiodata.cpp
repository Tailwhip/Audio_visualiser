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


audioData::audioData(QXYSeries *series, QObject *parent, bool d_type) :
    QIODevice(parent),
    data_series(series)
{
    dataType=d_type;
}
/**
 * @brief audioData::readData is a virtual function inherited from IODevice class. Unsused in this class.
 */
qint64 audioData::readData(char *data, qint64 maxSize)
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
qint64 audioData::writeData(const char *data, qint64 maxSize)
{
    if (dataType==1)
    {
        return fftData(f_resolution, sampleRate, f_sampleCount, data, maxSize);
    }
    else
        return timeData(t_resolution, t_sampleCount, data, maxSize);
}

qint64 audioData::timeData(int resolution, int sampleCount, const char *&audio_data, qint64 &max_size)
{
    createBuffer(sampleCount);

    int start = 0;
    const int availableSamples = int(max_size) / resolution;
    if (availableSamples < sampleCount)
    {
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            buffer[s].setY(buffer.at(s + availableSamples).y());
    }

    for (int s = start; s < sampleCount; ++s, audio_data += resolution)
        buffer[s].setY(qreal(uchar(*audio_data)-128) / qreal(128));


    data_series->replace(buffer);
    return (sampleCount - start) * resolution;
}

qint64 audioData::fftData(int resolution, int sampleRate, int sampleCount, const char *&audio_data, qint64 &max_size)
{
    //creating a buffer:
    createBuffer(sampleCount);

    int start = 0;

    const int availableSamples = int(max_size) / resolution;
    if (availableSamples < sampleCount)
    {
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            buffer[s].setY(buffer.at(s + availableSamples).y());
    }

    //adding harvested samples to buffer:
    for (int s = start; s < sampleCount; ++s, audio_data+= resolution)
    {
        buffer[s].setY(qreal(uchar(*audio_data)-128) / qreal(128));
    }

    //making complex plan:
    fftw_complex in[sampleCount];
    fftw_complex out[sampleCount];
    for (int i = 0; i < sampleCount; ++i)
    {
        //adding a window function and data to complex plan:
        in[i][REAL] = buffer[i].y()*Hann_window(i, sampleCount);
        in[i][IMAG] = 0;
    }
    //Fourier transformation:
    fftw_plan plan=fftw_plan_dft_1d(sampleCount, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    /* since only half of computed fft data is needed (rest is mirrored)
       a proper costants is beeing setted: */
    int halfBuffer=(sampleCount/2);
    double dB_magnitude[halfBuffer];
    //putting computed data into buffer:
    for (int i = 0; i < halfBuffer; ++i)
    {
        //std::cout<<out[i][REAL]<<std::endl;
        //compute magnitude [dB]:
        dB_magnitude[i]=10*log10(((out[i][REAL]*out[i][REAL]) + (out[i][IMAG]*out[i][IMAG])));
        //setting amplitude:
        buffer[i].setY(dB_magnitude[i]);
        //setting frequency domain:
        buffer[i].setX(i*(sampleRate/sampleCount));
        //std::cout<<dB_magnitude[i]<<"|"<<m_buffer[i].x()<<std::endl;
    }
    //cleaning up a plan:
    fftw_destroy_plan(plan);
    fftw_cleanup();
    //taking results out of function:
    data_series->replace(buffer);
    return (sampleCount - start) * resolution;
}

void audioData::createBuffer(int sampleCount)
{
    if (buffer.isEmpty())
    {
        buffer.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            buffer.append(QPointF(i, 0));
    }
}

double audioData::Hann_window(int iterator, int sampleCount)
{
    double multiplayer=0;
    return multiplayer=0.5*(1-cos(2*M_PI*iterator/(sampleCount-1)));
}
