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
#ifdef USE_PIXFC
#include "pixfc-sse.h"
#endif

class QImage;

namespace plaYUVer
{

class PlaYUVerFrame
{
public:
  PlaYUVerFrame( UInt width = 0, UInt height = 0, Int pel_format = 0 );
  PlaYUVerFrame( PlaYUVerFrame *other );
  PlaYUVerFrame( PlaYUVerFrame *other, QRect area );
  ~PlaYUVerFrame();

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

  static Int isRGBorYUVorGray( Int pixel_format )
  {
    switch( pixel_format )
    {
    case GRAY:
      return COLOR_GRAY;
      break;
    case YUV420p:
    case YUV444p:
    case YUV422p:
    case YUYV422:
      return COLOR_YUV;
      break;
    case RGB8:
      return COLOR_RGB;
      break;
    }
    return COLOR_INVALID;
  }

  static QStringList supportedPixelFormatList()
  {
    QStringList formats;
    formats << "YUV420p"
            << "YUV444p"
            << "YUV422p"
            << "YUYV422"
            << "GRAY"
            << "RGB8"  // RGB 3*8 bits per pixel
            ;
    assert( formats.size() == NUMBER_FORMATS );
    return formats;
  }

  Void FrametoRGB8();

  Void FrameFromBuffer( Pel*, Int );
  Void FrameToBuffer( Pel* );

  Void copyFrom( PlaYUVerFrame* );
  Void copyFrom( PlaYUVerFrame*, UInt, UInt);

  UInt64 getBytesPerFrame();

  UInt getChromaLength() const;

  Pel*** getPelBufferYUV() const
  {
    return m_pppcInputPel;
  }

  Pel*** getPelBufferYUV()
  {
    m_bHasRGBPel = false;
    return m_pppcInputPel;
  }

  Pixel getPixelValue( const QPoint&, Int );

  UChar* getQImageBuffer() const
  {
    return m_pcRGBPelInterlaced;
  }

  static Pixel ConvertPixel( Pixel, Int );

#ifdef USE_OPENCV
  cv::Mat getCvMat();
  Void copyFrom( cv::Mat* );
#endif

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
  Int getNumberChannels() const
  {
    return m_iNumberChannels;
  }
  Int getBitsChannel() const
  {
    return m_iBitsChannels;
  }
  Bool isValid() const
  {
    return ( m_uiWidth > 0 ) && ( m_uiHeight > 0 ) && ( m_iPixelFormat >= 0 );
  }

private:
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  Int m_iNumberChannels;
  Int m_iBitsChannels;

  Bool m_bHasRGBPel;
  Pel*** m_pppcInputPel;
  UChar* m_pcRGBPelInterlaced;

  Void init( UInt width, UInt height, Int pel_format );
  Void openPixfc();
  Void closePixfc();
#ifdef USE_PIXFC
  PixFcSSE* m_pcPixfc;
  inline Void FrametoRGB8Pixfc();
#endif
}
;

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__
