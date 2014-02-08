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

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif

#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif

#include "TypeDef.h"

#include "PlaYUVerFrame.h"

namespace plaYUVer
{

enum InputStream_Errors
{
  NO_STREAM_ERROR = 0, READING = 1, END_OF_SEQ = 2,
};

class InputStream
{
private:

  Int m_iStatus;
  Int m_iErrorStatus;

#ifdef USE_FFMPEG
  LibAvContextHandle m_cLibAvContext;
#endif

  QString m_cFilename;
  QString m_cStreamInformationString;

  FILE* m_pFile; /**< The input file pointer >*/
  Int m_iFileFormat;
  Int m_iPixelFormat;

  Pel* m_pInputBuffer;

  PlaYUVerFrame *m_cCurrFrame;

  UInt m_uiWidth;
  UInt m_uiHeight;
  UInt m_uiFrameRate;
  UInt m_uiTotalFrameNum;
  Int m_iCurrFrameNum;

public:
  InputStream();
  ~InputStream();

  static QString supportedReadFormats();
  static QString supportedWriteFormats();
  static QStringList supportedReadFormatsList();
  static QStringList supportedWriteFormatsList();

  enum InputStreamFormats
  {
    INVALID = -1, YUVFormat = 0,  // Use color space.
  };

  Bool needFormatDialog( QString filename );

  Void init( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate );
  Void close();

  Void readFrame();

  Bool writeFrame( const QString& filename );

  Void getFrame( QImage *qimage );
#ifdef USE_OPENCV
  cv::Mat getFrameCvMat();
#endif

  Void seekInput( Int new_frame_num );

  Bool checkErrors( Int error_type );

  Int getStatus()
  {
    return m_iStatus;
  }
  QString getFileName()
  {
    return m_cFilename;
  }
  QString getStreamInformationString()
  {
    return m_cStreamInformationString;
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
  Int getCurrFrameNum()
  {
    return m_iCurrFrameNum;
  }
  UInt getFrameRate()
  {
    return m_uiFrameRate;
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

}  // NAMESPACE

#endif // __INPUTSTREAM_H__
