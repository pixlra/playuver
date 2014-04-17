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
 * \file     PlaYUVerFrameStatistics.cpp
 * \brief    Measure PlaYUVerFrame statistics
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#include <cstdio>
#include <cmath>
#include <cstring>

#include <QtGui>
#include "PlaYUVerFrameStatistics.h"
#include "LibMemory.h"

namespace plaYUVer
{

////////////////////////////////////////////////////////////////////////////////
//                           Private Data Class
////////////////////////////////////////////////////////////////////////////////
class PlaYUVerFrameStatisticsPrivate
{

public:
  /*
   // Using a structure instead a class is more fast
   // (access with memset() and bytes manipulation).

   struct double_packet
   {
   double luma;
   double red;
   double green;
   double blue;
   double alpha;
   };
   */
public:

  PlaYUVerFrameStatisticsPrivate()
  {
    parent = 0;
    imageData = 0;
    histogram = 0;
    runningFlag = true;
  }

  /** The histogram data.*/
  Double *histogram;

  /** If true, calc the luminosity channel histogram when the image is RGB */
  Bool calcLuma;

  /** If the image is RGB and calcLuma is true, we have 1 more channel */
  Int histoChannels;

  /** Image information.*/
  Pel ***imageData;
  UInt imageWidth;
  UInt imageHeight;
  Int imageChannels;
  Int imageColorSpace;
  UInt imageChromaSize;

  /** Numbers of histogram segments dependaing of image bytes depth*/
  Int histoSegments;

  /** To post event from thread to parent.*/
  QObject *parent;

  /** Used to stop thread during calculations.*/
  Bool runningFlag;
};

////////////////////////////////////////////////////////////////////////////////
//                       Constructors and Destructor
////////////////////////////////////////////////////////////////////////////////

PlaYUVerFrameStatistics::PlaYUVerFrameStatistics( const PlaYUVerFrame *playuver_frame, QObject *parent, Options options ) :
        QThread()
{
  setup( playuver_frame->getPelBufferYUV(), playuver_frame->getWidth(), playuver_frame->getHeight(), playuver_frame->getBitsChannel(),
      playuver_frame->getPelFormat(), playuver_frame->getChromaLength(), parent, options );
}

PlaYUVerFrameStatistics::PlaYUVerFrameStatistics( Pel ***data, UInt width, UInt height, Int bitsPerChannel, Int pixel_format, UInt chroma_size, QObject *parent,
    Options options ) :
        QThread()
{
  setup( data, width, height, bitsPerChannel, pixel_format, chroma_size, parent, options );
}

PlaYUVerFrameStatistics::~PlaYUVerFrameStatistics()
{
  stopCalcHistogramValues();

  if( d->histogram )
    delete[] d->histogram;

  delete d;
}

////////////////////////////////////////////////////////////////////////////////
//                              Setup Function 
////////////////////////////////////////////////////////////////////////////////
void PlaYUVerFrameStatistics::setup( Pel ***data, UInt width, UInt height, Int bitsPerChannel, Int pixel_format, UInt chroma_size, QObject *parent,
    Options options )
{
  d = new PlaYUVerFrameStatisticsPrivate;
  d->imageData = data;
  d->imageWidth = width;
  d->imageHeight = height;
  d->parent = parent;
  d->histoSegments = ( bitsPerChannel == 16 ) ? 65536 : 256;
  d->imageColorSpace = PlaYUVerFrame::isRGBorYUVorGray( pixel_format );
  d->imageChromaSize = chroma_size;

  if( options == CalcLumaWhenRGB )
    d->calcLuma = true;
  else
    d->calcLuma = false;

  switch( d->imageColorSpace )
  {
  case PlaYUVerFrame::COLOR_GRAY:
    d->imageChannels = 1;
    break;
  case PlaYUVerFrame::COLOR_RGB:
  case PlaYUVerFrame::COLOR_YUV:
    d->imageChannels = 3;
    break;
  case PlaYUVerFrame::COLOR_ARGB:
    d->imageChannels = 4;
    break;
  default:
    d->imageChannels = 1;
  }

  if( d->calcLuma && ( d->imageColorSpace == PlaYUVerFrame::COLOR_RGB ) )
    d->histoChannels = d->imageChannels + 1;
  else
    d->histoChannels = d->imageChannels;

  if( d->imageData && d->imageWidth && d->imageHeight )
  {
    if( d->parent )
      start();
    else
      run();	//Lukas - substitui calcHistogramValues por run()
  }
  else
  {
    if( d->parent )
      postProgress( false, false );
  }
}
////////////////////////////////////////////////////////////////////////////////
//                         Accessing Properties
////////////////////////////////////////////////////////////////////////////////
int PlaYUVerFrameStatistics::getHistogramSegment( void )
{
  return d->histoSegments;
}

int PlaYUVerFrameStatistics::getRealHistoChannel( int channel )
{
  int histoChannel;

  switch( d->imageColorSpace )
  {
  case PlaYUVerFrame::COLOR_GRAY:
    if( channel != LumaChannel )
    {
      return -1;
    }
    histoChannel = 0;
    break;
  case PlaYUVerFrame::COLOR_RGB:
    if( channel != RedChannel && channel != GreenChannel && channel != BlueChannel && ( channel != LumaChannel && d->calcLuma ) )
    {
      return -1;
    }
    else
    {
      if( channel == RedChannel )
        histoChannel = 0;

      if( channel == GreenChannel )
        histoChannel = 1;

      if( channel == BlueChannel )
        histoChannel = 2;

      if( channel == LumaChannel )
        histoChannel = 3;
    }
    break;
  case PlaYUVerFrame::COLOR_ARGB:
    if( channel != RedChannel && channel != GreenChannel && channel != BlueChannel && channel != AlphaChannel && ( channel != LumaChannel && d->calcLuma ) )
    {
      return -1;
    }
    else
    {
      if( channel == AlphaChannel )
        histoChannel = 0;

      if( channel == RedChannel )
        histoChannel = 1;

      if( channel == GreenChannel )
        histoChannel = 2;

      if( channel == BlueChannel )
        histoChannel = 3;

      if( channel == LumaChannel )
        histoChannel = 4;
    }
    break;
  case PlaYUVerFrame::COLOR_YUV:
    if( channel != LumaChannel && channel != YChannel && channel != CbChannel && channel != CrChannel )
    {
      return -1;
    }
    else
    {
      if( channel == YChannel || channel == LumaChannel)
        histoChannel = 0;

      if( channel == CbChannel )
        histoChannel = 1;

      if( channel == CrChannel )
        histoChannel = 2;
    }
    break;
  default:
    histoChannel = -1;
  }
  return histoChannel;
}
////////////////////////////////////////////////////////////////////////////////
//                         Progress Indicators 
////////////////////////////////////////////////////////////////////////////////
void PlaYUVerFrameStatistics::postProgress( bool starting, bool success )
{
  EventData *eventData = new EventData();
  eventData->starting = starting;
  eventData->success = success;
  eventData->histogram = this;
  QCoreApplication::postEvent( d->parent, eventData );
}

void PlaYUVerFrameStatistics::stopCalcHistogramValues( void )
{
  d->runningFlag = false;
  wait();
}

////////////////////////////////////////////////////////////////////////////////
//                      List of threaded operations
////////////////////////////////////////////////////////////////////////////////
void PlaYUVerFrameStatistics::run()
{
  calcHistogramValues();
}

void PlaYUVerFrameStatistics::calcHistogramValues()
{
  register uint i;
  register int j;

  if( d->parent )
    postProgress( true, false );

  getMem1D<Double>( &( d->histogram ), d->histoSegments * d->histoChannels );

  if( !d->histogram )
  {
    qWarning( ) << "PlaYUVerFrameStatistics::calcHistogramValuess: "
                "Unable to allocate memory!"
                << endl;

    if( d->parent )
      postProgress( false, false );

    return;
  }

  if( d->imageColorSpace == PlaYUVerFrame::COLOR_YUV )
  {
    Pel *data[3];
    data[LUMA] = d->imageData[LUMA][0];
    data[CHROMA_U] = d->imageData[CHROMA_U][0];
    data[CHROMA_V] = d->imageData[CHROMA_V][0];
    for( i = 0; ( i < d->imageHeight * d->imageWidth ) && d->runningFlag; i++ )
    {
      d->histogram[*( data[LUMA] ) + LUMA * d->histoSegments]++;
      data[LUMA] += 1;
    }
    for( i = 0; ( i < d->imageChromaSize ) && d->runningFlag; i++ )
    {
      for( j = 1; j < d->imageChannels; j++ )
      {
        d->histogram[*( data[j] ) + j * d->histoSegments]++;
      }
      for( j = 1; j < d->imageChannels; j++ )
        data[j] += 1;
    }
  }
  else
  {
    Pel *data[3], luma;
    data[COLOR_R] = d->imageData[COLOR_R][0];
    data[COLOR_G] = d->imageData[COLOR_G][0];
    data[COLOR_B] = d->imageData[COLOR_B][0];

    for( i = 0; ( i < d->imageHeight * d->imageWidth ) && d->runningFlag; i++ )
    {
      for( j = 0; j < d->imageChannels; j++ )
      {
        d->histogram[*( data[j] ) + j * d->histoSegments]++;
      }
      if( d->calcLuma && ( d->imageColorSpace == PlaYUVerFrame::COLOR_RGB || d->imageColorSpace == PlaYUVerFrame::COLOR_ARGB ) )
      {
        Pixel pixel_value =
        {
            d->imageColorSpace,
            *( data[COLOR_R] ),
            *( data[COLOR_G] ),
            *( data[COLOR_B] ) };
        luma = PlaYUVerFrame::ConvertPixel( pixel_value, PlaYUVerFrame::COLOR_YUV ).Luma;
        d->histogram[luma + j * d->histoSegments]++;
      }
      for( j = 0; j < d->imageChannels; j++ )
        data[j] += 1;
    }
  }

  if( d->parent && d->runningFlag )
    postProgress( false, true );

}

double PlaYUVerFrameStatistics::getCount( int channel, int start, int end )
{
  int i;
  int histoChannel;
  int indexStart;
  double count = 0.0;

  if( !d->histogram || start < 0 || end > d->histoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->histoSegments;

  for( i = start; i <= end; i++ )
  {
    count += d->histogram[indexStart + i];
  }
  return count;
}

double PlaYUVerFrameStatistics::getPixels()
{
  if( !d->histogram )
  {
    return 0.0;
  }

  return ( d->imageWidth * d->imageHeight );
}

double PlaYUVerFrameStatistics::getMean( int channel, int start, int end )
{
  int i;
  int indexStart;
  int histoChannel;
  double mean = 0.0;
  double count;

  if( !d->histogram || start < 0 || end > d->histoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->histoSegments;

  for( i = start; i <= end; i++ )
  {
    mean += i * d->histogram[indexStart + i];
  }

  count = getCount( channel, start, end );

  if( count > 0.0 )
  {
    return mean / count;
  }

  return mean;
}

int PlaYUVerFrameStatistics::getMedian( int channel, int start, int end )
{
  int i;
  int histoChannel;
  int indexStart;
  double sum = 0.0;
  double count;

  if( !d->histogram || start < 0 || end > d->histoSegments - 1 || start > end )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->histoSegments;

  count = getCount( channel, start, end );

  for( i = start; i <= end; i++ )
  {
    sum += d->histogram[indexStart + i];
    if( sum * 2 > count )
      return i;
  }

  return 0;
}

double PlaYUVerFrameStatistics::getStdDev( int channel, int start, int end )
{
  int i;
  int histoChannel;
  int indexStart;
  double dev = 0.0;
  double count;
  double mean;

  if( !d->histogram || start < 0 || end > d->histoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->histoSegments;
  mean = getMean( channel, start, end );
  count = getCount( channel, start, end );

  if( count == 0.0 )
    count = 1.0;

  /*------------ original

   for ( i = start ; i <= end ; i++ )
   {
   dev += ( i - mean ) * ( i - mean ) * d->histogram[indexStart +i];
   }

   return sqrt( dev / count );

   -----------------------*/

  for( i = start; i <= end; i++ )
  {
    dev += ( i * i ) * d->histogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );

}

double PlaYUVerFrameStatistics::getValue( int channel, int bin )
{
  double value;
  int histoChannel;
  int indexStart;

  if( !d->histogram || bin < 0 || bin > d->histoSegments - 1 )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->histoSegments;

  value = d->histogram[indexStart + bin];

  return value;
}

double PlaYUVerFrameStatistics::getMaximum( int channel )
{
  double max = 0.0;
  int x;
  int histoChannel;
  int indexStart;

  if( !d->histogram )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->histoSegments;

  for( x = 0; x < d->histoSegments; x++ )
  {
    if( d->histogram[indexStart + x] > max )
    {
      max = d->histogram[indexStart + x];
    }
  }

  return max;
}

}  // NAMESPACE

