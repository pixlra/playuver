/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     StreamHandlerLibav.h
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
#include "PlaYUVerStream.h"
#include "PlaYUVerStreamHandlerIf.h"

namespace plaYUVer
{

class StreamHandlerLibav: public PlaYUVerStreamHandlerIf
{
  REGISTER_STREAM_HANDLER( StreamHandlerLibav )

public:

  static std::vector<PlaYUVerSupportedFormat> supportedReadFormats();
  static std::vector<PlaYUVerSupportedFormat> supportedWriteFormats();

  StreamHandlerLibav()
  {
  }
  ~StreamHandlerLibav()
  {
  }

  Bool openHandler( std::string strFilename, Bool bInput );
  Void closeHandler();
  Bool configureBuffer( PlaYUVerFrame* pcFrame );
  UInt64 calculateFrameNumber();
  Bool seek( UInt64 iFrameNum );
  Bool read( PlaYUVerFrame* pcFrame );
  Bool write( PlaYUVerFrame* pcFrame );

  UInt getStreamDuration()
  {
    return m_uiSecs;
  }

  UChar* m_pchFrameBuffer;
  UInt64 m_uiFrameBufferSize;

private:

  AVFormatContext* m_cFmtCtx;
  AVCodecContext* m_cCodecCtx;
  AVStream* m_cStream;
  Int m_iStreamIdx;
  AVFrame* m_cFrame;
  AVPacket pkt;

  Bool m_bHasStream;

  UInt64 m_uiSecs;
  UInt64 m_uiMicroSec;

  Bool decodeVideoPkt();
};

}  // NAMESPACE

#endif // __LIBAVCONTEXTHANDLE_H__
