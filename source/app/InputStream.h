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
 * \file     InputStream.h
 * \brief    Input stream handling
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

class InputStream
{
private:

  Bool m_bInit;
  Int m_iStatus;
  Int m_iErrorStatus;

#ifdef USE_FFMPEG
  LibAvContextHandle m_cLibAvContext;
#endif

  QString m_cFilename;
  QString m_cFormatName;
  QString m_cStreamInformationString;

  QTime m_cTimer;
  UInt m_uiAveragePlayInterval;

  FILE* m_pFile; /**< The input file pointer >*/
  Int m_iFileFormat;
  Int m_iPixelFormat;

  Pel* m_pInputBuffer;

  UInt m_uiFrameBufferSize;
  PlaYUVerFrame **m_ppcFrameBuffer;
  PlaYUVerFrame *m_pcCurrFrame;
  PlaYUVerFrame *m_pcNextFrame;
  UInt m_uiFrameBufferIndex;

  UInt m_uiWidth;
  UInt m_uiHeight;
  UInt m_uiFrameRate;
  UInt64 m_uiTotalFrameNum;
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
    INVALID = -1,
    YUVFormat = 0,  // Use color space.
  };

  enum InputStreamErrors
  {
    NO_STREAM_ERROR = 0,
    READING,
    LAST_FRAME,
    END_OF_SEQ,
  };

  Bool open( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate );
  Void close();

  Bool guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );
  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

  Void readNextFrame();

  Bool writeFrame( const QString& filename );

  Void setNextFrame();
  PlaYUVerFrame* getCurrFrame();
  PlaYUVerFrame* getCurrFrame( PlaYUVerFrame * );

  Void seekInput( UInt64 new_frame_num );

  Bool checkErrors( Int error_type );

  Bool isInit()
  {
    return m_bInit;
  }
  QString getFileName()
  {
    return m_cFilename;
  }
  QString getStreamInformationString()
  {
    return m_cStreamInformationString;
  }
  QString getFormatName()
  {
    return m_cFormatName;
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
