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

namespace plaYUVer
{

typedef struct
{
  const Char* name;
  Int colorSpace;
  UInt numberChannels;
  Int ratioChromaWidth;
  Int ratioChromaHeight;
  Pixel (*getPixelValue)( Pel ***Img, Int xPos, Int yPos );
  Void (*frameFromBuffer)( Pel *in, Pel*** out, UInt width, UInt height );
  Void (*bufferFromFrame)( Pel ***in, Pel* out, UInt width, UInt height );
  Void (*fillRGBbuffer)( Pel*** in, UChar* out, UInt width, UInt height );
  Int ffmpegPelFormat;
} PlaYUVerFramePelFormat;

#define PLAYUVER_NUMBER_FORMATS 6

extern PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS];

template<typename T>
Void yuvToRgb( T iY, T iU, T iV, T &iR, T &iG, T &iB )
{
  iR = iY + 1402 * iV / 1000;
  iG = iY - ( 101004 * iU + 209599 * iV ) / 293500;
  iB = iY + 1772 * iU / 1000;

  if( iR < 0 )
    iR = 0;
  if( iG < 0 )
    iG = 0;
  if( iB < 0 )
    iB = 0;

  if( iR > 255 )
    iR = 255;
  if( iG > 255 )
    iG = 255;
  if( iB > 255 )
    iB = 255;
}

template<typename T>
Void rgbToYuv( T iR, T iG, T iB, T &iY, T &iU, T &iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

inline Q_DECL_CONSTEXPR Int qRed(UInt rgb)             // get red part of RGB
{ return ((rgb >> 16) & 0xff); }

inline Q_DECL_CONSTEXPR Int qGreen(UInt rgb)           // get green part of RGB
{ return ((rgb >> 8) & 0xff); }

inline Q_DECL_CONSTEXPR Int qBlue(UInt rgb)            // get blue part of RGB
{ return (rgb & 0xff); }

inline Q_DECL_CONSTEXPR Int qAlpha(UInt rgb)           // get alpha part of RGBA
{ return rgb >> 24; }

inline Q_DECL_CONSTEXPR UInt qRgb(Int r, Int g, Int b) // set RGB value
{ return (0xffu << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff); }


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
  static QStringList supportedPixelFormatList();

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

#ifdef USE_OPENCV
  cv::Mat getCvMat();
  Void copyFrom( cv::Mat* );
#endif

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

  PlaYUVerFramePelFormat* m_pcPelFormat;

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
