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
 * \file     PlaYUVerFrameStats.h
 * \ingroup  PlaYUVerLib
 * \brief    Measure PlaYUVerFrame statistics
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __PLAYUVERFRAMESTATS_H__
#define __PLAYUVERFRAMESTATS_H__

#include "PlaYUVerDefs.h"

namespace plaYUVer
{

class PlaYUVerFrame;

class PlaYUVerFrameStats
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

  enum StatisticsOptions
  {
    CalcLumaWhenRGB = 1
  };

public:
  PlaYUVerFrameStats();
  ~PlaYUVerFrameStats();

  Void calcHistogram();

  // Methods for to manipulate the histogram data.
  Double getCount( Int channel, UInt start, UInt end );
  Double getMean( Int channel, UInt start, UInt end );
  Int getMedian( Int channel, UInt start, UInt end );

  /**
   * @return Number of image pixels
   */
  Double getPixels();
  Double getStdDev( Int channel, UInt start, UInt end );
  Double getHistogramValue( Int channel, UInt bin );
  Double getMaximum( Int channel );

  /**
   * @return Number of histogram segments: 256 or 65536 for 8 or 16 bits per
   *         sample.
   */
  Int getHistogramSegment();

  /**
   * Control calculation
   */
  Void setRunningFlag( Bool bFlag )
  {
    m_bRunningFlag = bFlag;
  }
  Bool getHasHistogram()
  {
    return m_bHasHistogram;
  }
private:

  /** The histogram data.*/
  Double* m_pdHistogram;

  /** If true, calc the luminosity channel histogram when the image is RGB */
  Bool calcLuma;

  /** If the image is RGB and calcLuma is true, we have 1 more channel */
  Int histoChannels;

  /** Image information.*/
  Pel*** m_pppImageData;
  UInt m_uiImageWidth;
  UInt m_uiImageHeight;
  UInt m_uiChannels;
  Int imageColorSpace;
  UInt imageChromaSize;

  /** Numbers of histogram segments dependaing of image bytes depth*/
  UInt m_uiHistoSegments;

  /** Used to stop thread during calculations.*/
  Bool m_bRunningFlag;

  Int getRealHistoChannel( Int channel );

protected:

  Bool m_bHasHistogram;

  Void xSetupStatistics( const PlaYUVerFrame *pcFrame, UInt options = CalcLumaWhenRGB );

};

}  // NAMESPACE

#endif // __PLAYUVERFRAMESTATS_H__
