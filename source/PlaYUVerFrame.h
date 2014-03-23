/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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

#include <QtCore>

#include "TypeDef.h"

class QImage;

namespace plaYUVer
{

class PlaYUVerFrame
{
public:
  PlaYUVerFrame( UInt width, UInt height, Int pel_format );
  ~PlaYUVerFrame();

  static QStringList supportedPixelFormatList();

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
  Pixel getPixelValue(const QPoint &pos, ColorSpace color);

  QImage getQimage();

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
};

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__
