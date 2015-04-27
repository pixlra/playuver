/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
 *                                Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     PlaYUVerFrameStats.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Measure PlaYUVerFrame statistics
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#include <cstdio>
#include <cmath>
#include <cstring>

#include "PlaYUVerFrameStats.h"
#include "PlaYUVerFrame.h"
#include "LibMemory.h"

namespace plaYUVer
{

PlaYUVerFrameStats::PlaYUVerFrameStats() :
        m_puiHistogram( NULL ),
        m_bHasHistogram( false )
{
}

PlaYUVerFrameStats::~PlaYUVerFrameStats()
{
  if( m_puiHistogram )
  {
    delete [] m_puiHistogram;
    //freeMem1D( m_puiHistogram );
  }
  m_puiHistogram = NULL;
  m_bHasHistogram = false;
}

// statistics
Void PlaYUVerFrameStats::xSetupStatistics( const PlaYUVerFrame *pcFrame, UInt options )
{
  m_pppImageData = pcFrame->getPelBufferYUV();
  m_uiImageWidth = pcFrame->getWidth();
  m_uiImageHeight = pcFrame->getHeight();
  m_uiChannels = pcFrame->getNumberChannels();

  m_uiHistoSegments = 1 << pcFrame->getBitsPel();
  imageColorSpace = pcFrame->getColorSpace();
  imageChromaSize = pcFrame->getChromaLength();

  if( options == CalcLumaWhenRGB )
    calcLuma = true;
  else
    calcLuma = false;

  if( calcLuma && ( imageColorSpace == PlaYUVerFrame::COLOR_RGB ) )
    histoChannels = m_uiChannels + 1;
  else
    histoChannels = m_uiChannels;
}

Void PlaYUVerFrameStats::calcHistogram()
{
  if( m_bHasHistogram )
    return;

  m_bHasHistogram = new UInt[m_uiHistoSegments * histoChannels];
  //getMem1D<UInt>( &( m_puiHistogram ), m_uiHistoSegments * histoChannels );
  if( !m_puiHistogram )
  {
//    qWarning() << "PlaYUVerFrameStats::calcHistogramValuess: "
//        "Unable to allocate memory!" << endl;
    return;
  }

  m_bRunningFlag = true;

  UInt i, j;

  if( imageColorSpace == PlaYUVerFrame::COLOR_YUV )
  {
    const Pel *data[3];
    data[LUMA] = m_pppImageData[LUMA][0];
    data[CHROMA_U] = m_pppImageData[CHROMA_U][0];
    data[CHROMA_V] = m_pppImageData[CHROMA_V][0];
    for( i = 0; i < m_uiImageHeight * m_uiImageWidth && m_bRunningFlag; i++ )
    {
      m_puiHistogram[*( data[LUMA] ) + LUMA * m_uiHistoSegments]++;
      data[LUMA] += 1;
    }
    for( i = 0; ( i < imageChromaSize ) && m_bRunningFlag; i++ )
    {
      for( j = 1; j < m_uiChannels; j++ )
      {
        m_puiHistogram[*( data[j] ) + j * m_uiHistoSegments]++;
      }
      for( j = 1; j < m_uiChannels; j++ )
        data[j] += 1;
    }
  }
  else
  {
    const Pel* data[3];
    Pel luma;
    data[COLOR_R] = m_pppImageData[COLOR_R][0];
    data[COLOR_G] = m_pppImageData[COLOR_G][0];
    data[COLOR_B] = m_pppImageData[COLOR_B][0];

    for( i = 0; ( i < m_uiImageHeight * m_uiImageWidth ) && m_bRunningFlag; i++ )
    {
      for( j = 0; j < m_uiChannels; j++ )
      {
        m_puiHistogram[*( data[j] ) + j * m_uiHistoSegments]++;
      }
      if( calcLuma && ( imageColorSpace == PlaYUVerFrame::COLOR_RGB || imageColorSpace == PlaYUVerFrame::COLOR_ARGB ) )
      {
        PlaYUVerFrame::Pixel currPixel( imageColorSpace, *( data[COLOR_R] ), *( data[COLOR_G] ), *( data[COLOR_B] ) );
        luma = PlaYUVerFrame::ConvertPixel( currPixel, PlaYUVerFrame::COLOR_YUV ).Y();
        m_puiHistogram[luma + j * m_uiHistoSegments]++;
      }
      for( j = 0; j < m_uiChannels; j++ )
        data[j] += 1;
    }
  }
  m_bHasHistogram = true;
  m_bRunningFlag = false;
}

Int PlaYUVerFrameStats::getHistogramSegment()
{
  return m_uiHistoSegments;
}

Int PlaYUVerFrameStats::getRealHistoChannel( Int channel )
{
  Int histoChannel;

  switch( imageColorSpace )
  {
  case PlaYUVerFrame::COLOR_GRAY:
    if( channel != LumaChannel )
    {
      return -1;
    }
    histoChannel = 0;
    break;
  case PlaYUVerFrame::COLOR_RGB:
    if( channel != RedChannel && channel != GreenChannel && channel != BlueChannel && ( channel != LumaChannel && calcLuma ) )
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
    if( channel != RedChannel && channel != GreenChannel && channel != BlueChannel && channel != AlphaChannel && ( channel != LumaChannel && calcLuma ) )
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
      if( channel == YChannel || channel == LumaChannel )
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

Double PlaYUVerFrameStats::getCount( Int channel, UInt start, UInt end )
{
  UInt i;
  Int histoChannel;
  Int indexStart;
  Double count = 0.0;

  if( !m_puiHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( i = start; i <= end; i++ )
  {
    count += m_puiHistogram[indexStart + i];
  }
  return count;
}

Double PlaYUVerFrameStats::getPixels()
{
  if( !m_puiHistogram )
  {
    return 0.0;
  }

  return ( m_uiImageWidth * m_uiImageHeight );
}

Double PlaYUVerFrameStats::getMean( Int channel, UInt start, UInt end )
{
  Int indexStart;
  Int histoChannel;
  Double mean = 0.0;
  Double count;

  if( !m_puiHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( UInt i = start; i <= end; i++ )
  {
    mean += i * m_puiHistogram[indexStart + i];
  }

  count = getCount( channel, start, end );

  if( count > 0.0 )
  {
    return mean / count;
  }

  return mean;
}

Int PlaYUVerFrameStats::getMedian( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double sum = 0.0;
  Double count;

  if( !m_puiHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  count = getCount( channel, start, end );

  for( UInt i = start; i <= end; i++ )
  {
    sum += m_puiHistogram[indexStart + i];
    if( sum * 2 > count )
      return i;
  }

  return 0;
}

Double PlaYUVerFrameStats::getStdDev( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double dev = 0.0;
  Double count;
  Double mean;

  if( !m_puiHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;
  mean = getMean( channel, start, end );
  count = getCount( channel, start, end );

  if( count == 0.0 )
    count = 1.0;

  /*------------ original

   for ( i = start ; i <= end ; i++ )
   {
   dev += ( i - mean ) * ( i - mean ) * m_puiHistogram[indexStart +i];
   }

   return sqrt( dev / count );

   -----------------------*/

  for( UInt i = start; i <= end; i++ )
  {
    dev += ( i * i ) * m_puiHistogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );

}

Double PlaYUVerFrameStats::getHistogramValue( Int channel, UInt bin )
{
  Double value;
  Int histoChannel;
  Int indexStart;

  if( !m_puiHistogram || bin < 0 || bin > m_uiHistoSegments - 1 )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  value = m_puiHistogram[indexStart + bin];

  return value;
}

Double PlaYUVerFrameStats::getMaximum( Int channel )
{
  Double max = 0.0;

  Int histoChannel;
  Int indexStart;

  if( !m_puiHistogram )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( UInt x = 0; x < m_uiHistoSegments; x++ )
  {
    if( m_puiHistogram[indexStart + x] > max )
    {
      max = m_puiHistogram[indexStart + x];
    }
  }

  return max;
}

}  // NAMESPACE

