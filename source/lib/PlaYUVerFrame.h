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

#include "config.h"

#include <iostream>
#include <cstdio>
#include <cassert>
#include <QtCore>

#include "TypeDef.h"

class QImage;

namespace plaYUVer
{

static const QString gInputStreamFormatsNames[] =
{
    "YUV420p",
    "YUV444p",
    "YUV422p",
    "YUV411p",
    "YUV410p",
    "GRAY" };

class PlaYUVerFrame
{
public:
  PlaYUVerFrame( UInt width, UInt height, Int pel_format );
  ~PlaYUVerFrame();

  enum PixelFormats
  {
    NO_FMT = -1,
    YUV420p = 0,
    YUV444p,
    YUV422p,
    YUV411p,
    YUV410p,
    GRAY,
    RGB8,
    NUMBER_FORMATS
  };

  static QStringList supportedPixelFormatList()
  {
    QStringList formats;
    formats << "YUV420p"
            << "YUV444p"
            << "YUV411p"
            << "YUV411p"
            << "YUV410p"
            << "GRAY"
            << "RGB8"  // RGB 3*8 bits per pixel
            ;
    assert( formats.size() == NUMBER_FORMATS );
    return formats;
  }

  Void FrametoRGB8();

  Void FrameFromBuffer( Pel *input_buffer, Int pel_format );
  Void CopyFrom( PlaYUVerFrame* );

  UInt64 getBytesPerFrame();

  Pel*** getPelBufferYUV()
  {
    return m_pppcInputPel;
  }
  Pel*** getPelBufferRGB()
  {
    return m_pppcRGBPel;
  }

  static Pixel ConvertPixel( Pixel sInputPixel, ColorSpace eOutputSpace );
  Pixel getPixelValue( const QPoint &pos, ColorSpace color );

  UChar* getQImageBuffer()
  {
    return m_pcRGBPelInterlaced;
  }

#ifdef USE_OPENCV
  cv::Mat getCvMat();
#endif

  UInt getWidth()
  {
    return m_uiWidth;
  }
  UInt getHeight()
  {
    return m_uiHeight;
  }
  Int getPelFormat()
  {
    return m_iPixelFormat;
  }

private:
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;

  Bool m_bHasRGBPel;
  Pel** m_pppcInputPel[3];
  Pel*** m_pppcRGBPel;
  UChar* m_pcRGBPelInterlaced;
};

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__