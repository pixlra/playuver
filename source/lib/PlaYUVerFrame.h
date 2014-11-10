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
 * \file     PlaYUVerFrame.h
 * \brief    Video Frame handling
 */

#ifndef __PLAYUVERFRAME_H__
#define __PLAYUVERFRAME_H__

#ifdef PLAYUVER
#include "config.h"
#endif
#include "PlaYUVerDefs.h"
#include <iostream>
#include <cstdio>
#include <cassert>
#include <QtCore>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif

class PixFcSSE;
struct AVFrame;


namespace plaYUVer
{


class PlaYUVerFrame
{
public:
  enum ColorSpace
  {
    COLOR_INVALID = -1,
    COLOR_YUV = 0,
    COLOR_RGB,
    COLOR_ARGB,
    COLOR_GRAY,
  };

  enum PixelFormats
  {
    NO_FMT = -1,
    YUV420p = 0,
    YUV444p,
    YUV422p,
    YUYV422,
    GRAY,
    RGB8,
    NUMBER_FORMATS
  };

  static Int isRGBorYUVorGray( Int pixel_format );
  static QStringList supportedPixelFormatListNames();

  PlaYUVerFrame( UInt width = 0, UInt height = 0, Int pel_format = 0 );
  PlaYUVerFrame( PlaYUVerFrame *other );
  PlaYUVerFrame( PlaYUVerFrame *other, QRect area );
  ~PlaYUVerFrame();

  UInt64 getBytesPerFrame();
  static UInt64 getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat );

  Int getColorSpace() const;
  Int getNumberChannels() const;
  UInt getChromaWidth() const;
  UInt getChromaHeight() const;
  UInt getChromaLength() const;

  Void FrameFromBuffer( Pel*, Int );
  Void FrameToBuffer( Pel* );

  Void fillRGBBuffer();

  Void copyFrom( PlaYUVerFrame* );
  Void copyFrom( PlaYUVerFrame*, UInt, UInt );

  Pixel getPixelValue( Int xPos, Int yPos, Int eColorSpace );
  static Pixel ConvertPixel( Pixel, Int );

  UInt getWidth() const
  {
    return m_uiWidth;
  }
  UInt getHeight() const
  {
    return m_uiHeight;
  }
  Int getPelFormat() const
  {
    return m_iPixelFormat;
  }

  Int getBitsChannel() const
  {
    return m_iBitsChannels;
  }
  Bool isValid() const
  {
    return ( m_uiWidth > 0 ) && ( m_uiHeight > 0 ) && ( m_iPixelFormat >= 0 );
  }
  Bool haveSameFmt( PlaYUVerFrame* other ) const
  {
    return ( m_uiWidth == other->getWidth() ) && ( m_uiHeight == other->getHeight() ) && ( m_iPixelFormat == other->getPelFormat() );
  }
  Pel*** getPelBufferYUV() const
  {
    return m_pppcInputPel;
  }
  Pel*** getPelBufferYUV()
  {
    m_bHasRGBPel = false;
    return m_pppcInputPel;
  }
  UChar* getRGBBuffer() const
  {
    return m_pcRGB32;
  }

  /**
   * Interface to other libs
   */
  Void copyFrom( struct AVFrame * );

#ifdef USE_OPENCV
  cv::Mat getCvMat();
  Void copyFrom( cv::Mat* );
#endif

  /**
   * Quality metrics interface
   */

  enum QualityMetrics
  {
    NO_METRIC = -1,
    PSNR_METRIC = 0,
    MSE_METRIC,
    SSIM_METRIC,
    NUMBER_METRICS,
  };

  static QStringList supportedQualityMetricsList()
  {
    QStringList metrics;
    metrics << "PSNR"
            << "MSE"
            << "SSIM"  // SSIM
            ;
    assert( metrics.size() == NUMBER_METRICS );
    return metrics;
  }

  Double getQuality( Int Metric, PlaYUVerFrame* Org, Int component );
  Double getMSE( PlaYUVerFrame* Org, Int component );
  Double getPSNR( PlaYUVerFrame* Org, Int component );
  Double getSSIM( PlaYUVerFrame* Org, Int component );

private:

  struct structPlaYUVerFramePelFormat* m_pcPelFormat;

  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  Int m_iNumberChannels;
  Int m_iBitsChannels;

  Bool m_bHasRGBPel;
  Pel*** m_pppcInputPel;
  UChar* m_pcRGB32;

  Void adjustSelectedAreaDims( QRect &area, Int pel_format );
  Void init( UInt width, UInt height, Int pel_format );
  Void openPixfc();
  Void closePixfc();
  PixFcSSE* m_pcPixfc;
  Void FrametoRGB8Pixfc();
};

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__
