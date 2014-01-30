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
 * \file     InputStream.h
 * \brief    Input handling
 */

#ifndef __INPUTSTREAM_H__
#define __INPUTSTREAM_H__

#include "config.h"

#include <iostream>
#include <cstdio>

#include <QtCore>
#include <QtGui>

#include "TypeDef.h"

namespace plaYUVer
{

enum InputStream_Errors
{
  NO_ERROR = 0, READING = 1,
};

class InputStream
{
private:

  Int m_iStatus;
  Int m_iErrorStatus;

  QString m_cFilename;
  FILE* m_pFile; /**< The input file pointer >*/
  Int m_iFileFormat;
  Int m_iPixelFormat;

  UInt m_uiWidth;
  UInt m_uiHeight;
  UInt m_uiTotalFrameNum;
  UInt m_uiCurrFrameNum;

  Pel** m_pppcInputPel[3];
  Pel*** m_pppcRGBPel;

public:
  InputStream();
  ~InputStream();

  static QString supportedReadFormats();
  static QStringList supportedReadFormatsList();
  static QStringList supportedPixelFormatList();

  enum InputStreamColorSpace
  {
    YUV420,
    YUV400,
  };

  enum InputStreamFormats
  {
    INVALID = -1,
    YUVFormat = 0, // Use color space.
  };

  Void init( QString filename, UInt width, UInt height, Int input_format );

  Void YUV420toRGB();

  Void readFrame();

  QImage getFrameQImage();

  Void seekInput( Int new_frame_num );

  Bool checkErrors( Int error_type );

  Int getStatus()
  {
    return m_iStatus;
  }
  UInt getFrameNum()
  {
    return m_uiTotalFrameNum;
  }
  UInt getWidth() const
  {
    return m_uiWidth;
  }
  UInt getHeight() const
  {
    return m_uiHeight;
  }
  UInt getCurrFrameNum()
  {
    return m_uiCurrFrameNum;
  }

  Void setWidth( UInt new_width )
  {
    if( new_width > 0 && new_width % 2 == 0 )
    {
      m_uiWidth = new_width;
    }
  }
  Void setHeight( UInt new_height )
  {
    if( new_height > 0 && new_height % 2 == 0 )
    {
      m_uiHeight = new_height;
    }
  }

};

} // NAMESPACE

#endif // __INPUTSTREAM_H__
