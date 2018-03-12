/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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
 * \ingroup  CalypStreamGrp
 * \brief    interface with libav libs
 */

#ifndef __STREAMHANDLERLIBAV_H__
#define __STREAMHANDLERLIBAV_H__

#include <inttypes.h>
#include <string>
#include <vector>

#ifndef __PRI64_PREFIX
#define __PRI64_PREFIX "l"
#endif

#ifndef PRId64
#define PRId64 __PRI64_PREFIX "d"
#endif

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#include "CalypStream.h"
#include "CalypStreamHandlerIf.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVStream;
struct AVPacket;

class StreamHandlerLibav : public CalypStreamHandlerIf
{
  REGISTER_CALYP_STREAM_HANDLER( StreamHandlerLibav )

public:
  StreamHandlerLibav();
  ~StreamHandlerLibav() {}
  bool openHandler( ClpString strFilename, bool bInput );
  void closeHandler();
  bool configureBuffer( CalypFrame* pcFrame );
  void calculateFrameNumber();
  bool seek( unsigned long long int iFrameNum );
  bool read( CalypFrame* pcFrame );
  bool write( CalypFrame* pcFrame );

  unsigned int getStreamDuration() { return m_uiSecs; }
  unsigned char* m_pchFrameBuffer;
  unsigned long long int m_uiFrameBufferSize;

private:
  AVFormatContext* m_cFmtCtx;
  AVStream* m_cStream;
  int m_iStreamIdx;

  struct SwsContext* m_ScalerCtx;

  AVCodecContext* m_cCodedCtx;

  int m_ffPixFmt;
  AVFrame* m_cFrame;
  AVPacket m_cOrgPacket;
  AVPacket m_cPacket;

  bool m_bHasStream;

  unsigned long long int m_uiSecs;
  unsigned long long int m_uiMicroSec;

  AVFrame* m_cConvertedFrame;
};

#endif  // __STREAMHANDLERLIBAV_H__
