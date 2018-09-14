#include "fftdata.h"
#include <QtCharts/QXYSeries>
#include <fftw3.h>
#include <math.h>
#include <iostream>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define REAL 0
#define IMAG 1


/**
 * @brief FFTData::FFTData is the constructor of class FFTData.
 * @param series are audio data samples collected from the outer device (microphone).
 * @param parent returns a pointer to the parent object.
 */
FFTData::FFTData(QXYSeries *series, QObject *parent) :
    QIODevice(parent),
    m_series(series)
{
}

qint64 FFTData::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}
/**
 * @brief FFTData::writeData is a virtual function inherited from IODevice class. Writes up to maxSize bytes from data to
 * the m_series buffer. Collects audio data and make them usefull for plotting in frequency domain.
 * @param data contains collected audio samples.
 * @param maxSize is equal to maximum bytes size of collected data.
 */
qint64 FFTData::writeData(const char *data, qint64 maxSize)
{
    static const int resolution = 2;
    //creating a buffer:
    if (m_buffer.isEmpty())
    {
        m_buffer.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
        {
            m_buffer.append(QPointF(i, 0));
        }
    }
    int start = 0;

    const int availableSamples = int(maxSize) / resolution;
    if (availableSamples < sampleCount)
    {
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s)
            m_buffer[s].setY(m_buffer.at(s + availableSamples).y());
    }

    //adding harvested samples to buffer:
    for (int s = start; s < sampleCount; ++s, data += resolution)
    {
        m_buffer[s].setY(qreal(uchar(*data)-128) / qreal(128));
    }

    //making complex plan:
    fftw_complex in[sampleCount];
    fftw_complex out[sampleCount];
    for (int i = 0; i < sampleCount; ++i)
    {
        //adding a window function and data to complex plan:
        in[i][REAL] = m_buffer[i].y()*Hann_window(i);
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
        m_buffer[i].setY(dB_magnitude[i]);
        //setting frequency domain:
        m_buffer[i].setX(i*(sampleRate/sampleCount));
        //std::cout<<dB_magnitude[i]<<"|"<<m_buffer[i].x()<<std::endl;
    }
    //cleaning up a plan:
    fftw_destroy_plan(plan);
    fftw_cleanup();
    //taking results out of function:
    m_series->replace(m_buffer);
    return (sampleCount - start) * resolution;
}

double FFTData::Hann_window(int iterator)
{
    double multiplayer=0;
    return multiplayer=0.5*(1-cos(2*M_PI*iterator/(sampleCount-1)));
}
