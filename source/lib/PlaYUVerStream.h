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
 * \file     PlaYUVerStream.h
 * \brief    Input stream handling
 */

#ifndef __PLAYUVERSTREAM_H__
#define __PLAYUVERSTREAM_H__

#include "PlaYUVerDefs.h"
#include <iostream>
#include <cstdio>
#include <QtCore>
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class LibAvContextHandle;

class PlaYUVerStream
{
private:

  Bool m_bInit;
  Bool m_bIsInput;
  Bool m_bLoadAll;
  Int m_iErrorStatus;

  LibAvContextHandle* m_cLibAvContext;

  QString m_cFilename;
  QString m_cFormatName;
  QString m_cCodedName;
  QString m_cStreamInformationString;

  QTime m_cTimer;
  UInt m_uiAveragePlayInterval;

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
  PlaYUVerStream();
  ~PlaYUVerStream();

  enum PlaYUVerStreamInputFormats
  {
    INVALID_INPUT = -1,
    YUVINPUT = 0,
#ifdef USE_FFMPEG
    AVIINPUT,
    MP4INPUT,
    WMVINPUT,
    PGMINPUT,
    PNGINPUT,
    BMPINPUT,
    JPEGINPUT,
#endif
    TOTAL_INPUT_FORMATS
  };

  static QStringList supportedReadFormatsExt()
  {
    QStringList formatsExt;
    formatsExt << "yuv"  // Raw video
#ifdef USE_FFMPEG
               << "avi"   // Audio video interleaved
               << "mp4"// MP4
               << "wmv"// Windows media video
               << "pgm"
               << "png"
               << "bmp"
               << "jpg"
#endif
               ;
    assert( formatsExt.size() == TOTAL_INPUT_FORMATS );
    return formatsExt;
  }

  static QStringList supportedReadFormatsName()
  {
    QStringList formatsName;
    formatsName << "Raw video"
#ifdef USE_FFMPEG
                << "Audio video interleaved"
                << "MPEG4"
                << "Windows media video"
                << "Portable Grayscale Map"
                << "Portable Network Graphics"
                << "Windows Bitmap"
                << "Joint Photographic Experts Group"
#endif
                ;
    assert( formatsName.size() == TOTAL_INPUT_FORMATS );
    return formatsName;
  }

  enum PlaYUVerStreamOutputFormats
  {
    INVALID_OUTPUT = -1,
    YUVOUTPUT = 0,
    TOTAL_OUTPUT_FORMATS
  };

  static QStringList supportedWriteFormatsExt()
  {
    QStringList formatsExt;
    formatsExt << "yuv"   // raw video
    ;
    assert( formatsExt.size() == TOTAL_OUTPUT_FORMATS );
    return formatsExt;
  }

  static QStringList supportedWriteFormatsName()
  {
    QStringList formatsName;
    formatsName << "Raw video";
    assert( formatsName.size() == TOTAL_OUTPUT_FORMATS );
    return formatsName;
  }

  static QStringList supportedSaveFormatsExt()
  {
    QStringList formatsExt;
    formatsExt << "bmp"
               << "jpeg"
               << "png"  // portable network graphics
               ;
    return formatsExt;
  }

  static QStringList supportedSaveFormatsName()
  {
    QStringList formatsName;
    formatsName << "Windows Bitmap"
                << "Joint Photographic Experts Group"
                << "Portable Network Graphics";
    return formatsName;
  }

#define REGIST_STANDARD_RESOLUTION( name, width, height) \
    stdRes.stringListName.append( QString( name ) ); \
    stdRes.stringListFullName.append( QString( "%1 (%2x%3)" ).arg( name ).arg( width ).arg( height ) ); \
    stdRes.sizeResolution.append( QSize( width, height ) )

  struct StandardResolution
  {
    QStringList stringListName;
    QStringList stringListFullName;
    QVector<QSize> sizeResolution;
  };

  static struct StandardResolution standardResolutionSizes()
  {
    struct StandardResolution stdRes;
    REGIST_STANDARD_RESOLUTION( "CIF", 352, 288 );
    REGIST_STANDARD_RESOLUTION( "VGA", 640, 480 );
    REGIST_STANDARD_RESOLUTION( "WVGA", 832, 480 );
    REGIST_STANDARD_RESOLUTION( "XVGA", 1024, 768 );
    REGIST_STANDARD_RESOLUTION( "HD", 1280, 720 );
    REGIST_STANDARD_RESOLUTION( "SXGA-", 1280, 900 );
    REGIST_STANDARD_RESOLUTION( "SXGA", 1280, 1024 );
    REGIST_STANDARD_RESOLUTION( "WSXGA", 1440, 900 );
    REGIST_STANDARD_RESOLUTION( "FullHD", 1920, 1080 );
    REGIST_STANDARD_RESOLUTION( "WQXGA", 2560, 1600 );
    REGIST_STANDARD_RESOLUTION( "UltraHD", 3840, 2160 );
    REGIST_STANDARD_RESOLUTION( "8K", 8192, 4608 );
    return stdRes;
  }

  enum PlaYUVerStreamErrors
  {
    NO_STREAM_ERROR = 0,
    READING,
    WRITING,
    LAST_FRAME,
    END_OF_SEQ,
  };

  Bool open( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate, Bool bInput = true );
  Void close();

  static Bool guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate );

  Void loadAll();

  Void readFrame();
  Void writeFrame();
  Void writeFrame( PlaYUVerFrame *pcFrame );

  Bool saveFrame( const QString& filename );

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
  QString getCodecName()
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
