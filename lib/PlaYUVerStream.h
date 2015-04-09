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
 * \ingroup  PlaYUVerLib
 * \brief    Input stream handling
 */

#ifndef __PLAYUVERSTREAM_H__
#define __PLAYUVERSTREAM_H__

#include "PlaYUVerDefs.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class LibAvContextHandle;
class LibOpenCVHandler;

typedef struct
{
  std::string shortName;
  UInt uiWidth;
  UInt uiHeight;
} PlaYUVerStdResolution;

/**
 * \class PlaYUVerStream
 * \ingroup  PlaYUVerLib PlaYUVerLib_Stream
 */
class PlaYUVerStream
{
private:

  Bool m_bInit;
  Bool m_bIsInput;
  Bool m_bIsOpened;

  Bool m_bLoadAll;

  enum PlaYUVerStreamHandlers
  {
    INVALID_HANDLER = -1,
    YUV_IO,
    OPENCV_HANDLER,
    FFMPEG,
    TOTAL_HANDLERR
  };
  Int m_iStreamHandler;
  LibAvContextHandle* m_cLibAvContext;

  std::string m_cFilename;
  Char* m_pchFilename;

  std::string m_cFormatName;
  std::string m_cCodedName;

  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  UInt m_uiBitsPerPixel;
  Double m_dFrameRate;
  UInt64 m_uiTotalFrameNum;
  Int64 m_iCurrFrameNum;

  FILE* m_pFile; /**< The input file pointer >*/
  std::fstream m_fsIOStream; /**< The input file pointer >*/
  Byte* m_pStreamBuffer;

  UInt m_uiFrameBufferSize;
  PlaYUVerFrame **m_ppcFrameBuffer;
  PlaYUVerFrame *m_pcCurrFrame;
  PlaYUVerFrame *m_pcNextFrame;
  UInt m_uiFrameBufferIndex;
  UInt64 m_uiCurrFrameFileIdx;

  Void findHandler();

public:

  static std::vector<std::string> supportedReadFormatsExt();
  static std::vector<std::string> supportedReadFormatsName();
  static std::vector<std::string> supportedWriteFormatsExt();
  static std::vector<std::string> supportedWriteFormatsName();
  static std::vector<std::string> supportedSaveFormatsExt();
  static std::vector<std::string> supportedSaveFormatsName();

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

  Bool open( std::string filename, std::string resolution, std::string input_format, UInt bitsPel, UInt frame_rate, Bool bInput = true );
  Bool open( std::string filename, UInt width, UInt height, Int input_format, UInt bitsPel, UInt frame_rate, Bool bInput = true );
  Void close();

  Bool openFile();
  Void closeFile();

  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, UInt& rFrameRate );

  Void loadAll();

  Void readFrame();
  Void readFrameFillRGBBuffer();
  Void writeFrame();
  Void writeFrame( PlaYUVerFrame *pcFrame );

  Bool saveFrame( const std::string& filename );
  static Bool saveFrame( const std::string& filename, PlaYUVerFrame *saveFrame );

  Bool setNextFrame();
  PlaYUVerFrame* getCurrFrame();
  PlaYUVerFrame* getCurrFrame( PlaYUVerFrame * );
  PlaYUVerFrame* getNextFrame();

  Bool seekInputRelative( Bool bIsFoward );
  Bool seekInput( UInt64 new_frame_num );

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
  Double getFrameRate()
  {
    return m_dFrameRate;
  }
  Void getDuration( Int* duration_array );

};

}  // NAMESPACE

#endif // __PLAYUVERSTREAM_H__
