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
 * \file     PlaYUVerStream.h
 * \brief    Input stream handling
 */

#ifndef __PLAYUVERSTREAM_H__
#define __PLAYUVERSTREAM_H__

#include "PlaYUVerDefs.h"
#include <iostream>
#include <cstdio>
#include <QStringList>
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class LibAvContextHandle;

typedef struct
{
  std::string shortName;
  UInt uiWidth;
  UInt uiHeight;
} PlaYUVerStdResolution;

class PlaYUVerStream
{
private:

  Bool m_bInit;
  Bool m_bIsInput;
  Bool m_bIsOpened;

  Bool m_bLoadAll;
  Int m_iErrorStatus;

  enum PlaYUVerStreamHandlers
  {
    INVALID_HANDLER = -1,
    YUV_IO,
    FFMPEG,
    TOTAL_HANDLERR
  };
  UInt m_uiStreamHandler;
  LibAvContextHandle* m_cLibAvContext;

  std::string m_cFilename;
  std::string m_cFormatName;
  std::string m_cCodedName;
  std::string m_cPelFmtName;


  FILE* m_pFile; /**< The input file pointer >*/
  Int m_iFileFormat;
  Int m_iPixelFormat;

  Pel* m_pStreamBuffer;

  UInt m_uiFrameBufferSize;
  PlaYUVerFrame **m_ppcFrameBuffer;
  PlaYUVerFrame *m_pcCurrFrame;
  PlaYUVerFrame *m_pcNextFrame;
  UInt m_uiFrameBufferIndex;

  UInt m_uiWidth;
  UInt m_uiHeight;
  UInt m_uiFrameRate;
  UInt64 m_uiTotalFrameNum;
  Int64 m_iCurrFrameNum;

public:

  static QStringList supportedReadFormatsExt();
  static QStringList supportedReadFormatsName();
  static QStringList supportedWriteFormatsExt();
  static QStringList supportedWriteFormatsName();
  static QStringList supportedSaveFormatsExt();
  static QStringList supportedSaveFormatsName();

  static std::vector<PlaYUVerStdResolution> stdResolutionSizes();

  PlaYUVerStream();
  ~PlaYUVerStream();

  enum PlaYUVerStreamErrors
  {
    NO_STREAM_ERROR = 0,
    READING,
    WRITING,
    LAST_FRAME,
    END_OF_SEQ,
  };

  Bool open( std::string filename, UInt width, UInt height, Int input_format, UInt frame_rate, Bool bInput = true );
  Void close();

  Bool openFile();
  Void closeFile();

  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

  Void loadAll();

  Void readFrame();
  Void writeFrame();
  Void writeFrame( PlaYUVerFrame *pcFrame );

  Bool saveFrame( const std::string& filename );
  static Bool saveFrame( const std::string& filename, PlaYUVerFrame *saveFrame );

  Void setNextFrame();
  PlaYUVerFrame* getCurrFrame();
  PlaYUVerFrame* getCurrFrame( PlaYUVerFrame * );

  Bool seekInput( UInt64 new_frame_num );

  Bool checkErrors( Int error_type );

  Bool isInit()
  {
    return m_bInit;
  }

  std::string getFileName()
  {
    return m_cFilename;
  }
  std::string getFormatName()
  {
    return m_cFormatName;
  }
  std::string getCodecName()
  {
    return m_cCodedName;
  }
  std::string getPelFmtName()
  {
    return m_cPelFmtName;
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
  Void getDuration( Int* duration_array );

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

#endif // __PLAYUVERSTREAM_H__
