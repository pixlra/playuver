/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     PlaYUVerFrameStatistics.h
 * \brief    Measure PlaYUVerFrame statistics
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __PLAYUVERFRAMESTATISTICS_H_
#define __PLAYUVERFRAMESTATISTICS_H_

#include "config.h"

#include <QThread>
#include "TypeDef.h"
#include "PlaYUVerFrame.h"

class QObject;

namespace plaYUVer
{

class PlaYUVerFrameStatisticsPrivate;

/**
 * Esta classe permite obter o histograma de uma imagem assim como algumas 
 * estatísticas. 
 */
class PlaYUVerFrameStatistics: public QThread
{

public:

  enum StatisticsChannel
  {
    LumaChannel = 0,
    RedChannel = 1,
    GreenChannel = 2,
    BlueChannel = 3,
    AlphaChannel = 4,
    YChannel = 1,
    CbChannel = 2,
    CrChannel = 3
  };

  enum Options
  {
    CalcLumaWhenRGB = 0
  };

  /**
   * Custom Event
   */
  class EventData: public QEvent
  {

  public:

    EventData() :
            QEvent( QEvent::User )
    {
      starting = false;
      success = false;
      histogram = 0;
    }

    bool starting;
    bool success;
    PlaYUVerFrameStatistics *histogram;
  };

public:

  PlaYUVerFrameStatistics( Pel ***data, UInt width, UInt height, Int bitsPerChannel, Int pixel_format, UInt chroma_size,
      QObject *parent = 0, Options options = CalcLumaWhenRGB );

  PlaYUVerFrameStatistics( const PlaYUVerFrame *playuver_frame, QObject *parent = 0, Options options = CalcLumaWhenRGB );

  ~PlaYUVerFrameStatistics();

  void setup( Pel ***data, UInt width, UInt height, Int bitsPerChannel, Int pixel_format, UInt chroma_size, QObject *parent = 0, Options options = CalcLumaWhenRGB );

  /** Method to stop threaded computations.*/
  void stopCalcHistogramValues( void );

  // Methods for to manipulate the histogram data.

  double getCount( int channel, int start, int end );
  double getMean( int channel, int start, int end );
  /**
   * @return Number of image pixels
   */
  double getPixels();
  double getStdDev( int channel, int start, int end );
  double getValue( int channel, int bin );
  double getMaximum( int channel );

  /**
   * @return Number of histogram segments: 256 or 65536 for 8 or 16 bits per
   *         sample.
   */
  int getHistogramSegment( void );
  int getMedian( int channel, int start, int end );

private:

  PlaYUVerFrameStatisticsPrivate *d;

private:

  int getRealHistoChannel( int channel );
  void calcHistogramValues();
  void postProgress( bool starting, bool success );

protected:

  virtual void run();
};

}  // NAMESPACE

#endif // __PLAYUVERFRAMESTATISTICS_H_
