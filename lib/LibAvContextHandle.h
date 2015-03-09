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
 * \file     LibAvContextHandle.h
 * \ingroup  PlaYUVerLib
 * \brief    Interface with libav libs
 */

#ifndef __LIBAVCONTEXTHANDLE_H__
#define __LIBAVCONTEXTHANDLE_H__

#include <inttypes.h>
#include <vector>
#include <string>

#ifndef __PRI64_PREFIX
#define __PRI64_PREFIX  "l"
#endif

#ifndef PRId64
#define PRId64   __PRI64_PREFIX "d"
#endif

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
//#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

#include "PlaYUVerDefs.h"

namespace plaYUVer
{

class LibAvContextHandle
{
public:

  static std::vector<std::string> supportedReadFormatsExt();
  static std::vector<std::string> supportedReadFormatsName();
  static std::vector<std::string> supportedWriteFormatsExt();
  static std::vector<std::string> supportedWriteFormatsName();
  static std::vector<std::string> supportedSaveFormatsExt();
  static std::vector<std::string> supportedSaveFormatsName();

  LibAvContextHandle() :
          m_bHasStream( false )
  {
  }
  Bool initAvFormat( const char* filename, UInt& width, UInt& height, Int& pixel_format, Double& frame_rate, UInt64& num_frames );
  Void closeAvFormat();
  Bool decodeAvFormat();

  Void seekAvFormat( UInt64 frame_num );

  Bool getStatus()
  {
    return m_bHasStream;
  }
  Char* getCodecName()
  {
    return m_acCodecName;
  }
  UInt getStreamDuration()
  {
    return m_uiSecs;
  }

//  uint8_t *video_dst_data[4];
//  int video_dst_linesize[4];
//  int video_dst_bufsize;

  UChar* m_pchFrameBuffer;
  UInt64 m_uiFrameBufferSize;

private:
  AVFormatContext *fmt_ctx;
  AVCodecContext *video_dec_ctx;
  AVStream *video_stream;
  Int video_stream_idx;
  AVFrame *frame;
  AVPacket pkt;

  Bool m_bHasStream;

  Char m_acCodecName[20];
  UInt64 m_uiSecs;
  UInt64 m_uiMicroSec;

  Bool decodeVideoPkt();
};

}  // NAMESPACE

#endif // __LIBAVCONTEXTHANDLE_H__
