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
extern "C"
{
#include "libavutil/imgutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/timestamp.h"
#include "libavformat/avformat.h"
}
#endif

#include "TypeDef.h"

namespace plaYUVer
{

enum InputStream_Errors
{
  NO_ERROR = 0, READING = 1,
};

#ifdef USE_FFMPEG
class LibAvContextHandle
{
public:
  Bool initAvFormat( QString filename, UInt width, UInt height );
  Void closeAvFormat();
  Bool decodeAvFormat();

  Bool getStatus()
  {
    return m_bHasStream;
  }

  uint8_t *video_dst_data[4];
  int video_dst_linesize[4];
  int video_dst_bufsize;

private:
  AVFormatContext *fmt_ctx;
  AVCodecContext *video_dec_ctx;
  AVStream *video_stream;
  Int video_stream_idx;
  AVFrame *frame;
  AVPacket pkt;

  Bool m_bHasStream;
};
#endif

class InputStream
{
private:

  Int m_iStatus;
  Int m_iErrorStatus;

#ifdef USE_FFMPEG
  LibAvContextHandle m_cLibAvContext;
#endif

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
  static QString supportedWriteFormats();
  static QStringList supportedReadFormatsList();
  static QStringList supportedWriteFormatsList();

  static QStringList supportedPixelFormatList();

  enum InputStreamColorSpace
  {
    YUV420, YUV400,
  };

  enum InputStreamFormats
  {
    INVALID = -1, YUVFormat = 0,  // Use color space.
  };

  Bool needFormatDialog( QString filename );

  Void init( QString filename, UInt width, UInt height, Int input_format );

  Void YUV420toRGB();

  Void readFrame();

  Bool writeFrame( const QString& filename );

  QImage getFrameQImage();
#ifdef USE_OPENCV
  cv::Mat getFrameCvMat();
#endif

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

}  // NAMESPACE

#endif // __INPUTSTREAM_H__
