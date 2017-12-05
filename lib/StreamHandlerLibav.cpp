/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     StreamHandlerLibav.cpp
 * \brief    Interface with libav libs
 */

#include "StreamHandlerLibav.h"
#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"

#include <cstdio>
#include <iostream>

#if( ( LIBAVCODEC_VERSION_MAJOR >= 57 ) && ( LIBAVCODEC_VERSION_MINOR >= 37 ) )
#define FF_SEND_RECEIVE_API
#endif

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Joint Photographic Experts Group", "jpg,jpeg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows Bitmap", "bmp" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Tagged Image File Format", "tiff" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Audio video interleaved", "avi" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows media video", "wmv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "MPEG4", "mp4" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Matroska Multimedia Container", "mkv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "H.264 streams", "264,h264" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "HEVC streams", "265,hevc" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

StreamHandlerLibav::StreamHandlerLibav()
{
  m_pchHandlerName = "FFmpeg";
}

Bool StreamHandlerLibav::openHandler( String strFilename, Bool bInput )
{
  const char* filename = strFilename.c_str();

  m_cFmtCtx = NULL;
  m_cStream = NULL;

  m_iStreamIdx = -1;
  m_cFrame = NULL;
  m_bHasStream = false;

  //	AVDictionary* format_opts = NULL;
  //	if( m_uiWidth > 0 && m_uiHeight > 0 )
  //	{
  //		Char aux_string[10];
  //		sprintf( aux_string, "%dx%d", m_uiWidth, m_uiHeight );
  //		av_dict_set( &format_opts, "video_size", aux_string, 0 );
  //	}
  //	Int ffmpeg_pel_format = g_PlaYUVerPixFmtDescriptorsList[m_iPixelFormat].ffmpegPelFormat;
  //	if( ffmpeg_pel_format >= 0 )
  //	{
  //		av_dict_set( &format_opts, "pix_fmt", av_get_pix_fmt_name( AVPixelFormat( ffmpeg_pel_format ) ), 0 );
  //	}

  // Register all components of FFmpeg
  av_register_all();

  /* open input file, and allocate format context */
  if( avformat_open_input( &m_cFmtCtx, filename, NULL, NULL ) < 0 )
  {
    std::cout << " Could not open source file %s !!!" << filename << std::endl;

    return false;
  }

  /* retrieve stream information */
  if( avformat_find_stream_info( m_cFmtCtx, NULL ) < 0 )
  {
    std::cout << " Could not find stream information !!!" << std::endl;
    return false;
  }

  int stream_idx = av_find_best_stream( m_cFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0 );
  if( stream_idx < 0 )
  {
    return false;
  }

  m_iStreamIdx = stream_idx;

  m_cStream = m_cFmtCtx->streams[m_iStreamIdx];

  /* find decoder for the stream */
  AVCodec* dec = avcodec_find_decoder( m_cStream->codecpar->codec_id );
  if( !dec )
  {
    std::cout << "Failed to find video coded" << std::endl;
    return false;
  }

  /* Allocate a codec context for the decoder */
  m_cCodedCtx = avcodec_alloc_context3( dec );
  if( !m_cCodedCtx )
  {
    std::cout << "Failed to allocate the video codec context" << std::endl;
    return false;
  }

  /* Copy codec parameters from input stream to output codec context */
  if( avcodec_parameters_to_context( m_cCodedCtx, m_cStream->codecpar ) < 0 )
  {
    std::cout << "Failed to copy video codec parameters to decoder context" << std::endl;
    return false;
  }

  if( avcodec_open2( m_cCodedCtx, dec, NULL ) < 0 )
  {
    std::cout << "Failed to open video coded" << std::endl;
    return false;
  }

  m_uiFrameBufferSize =
      av_image_get_buffer_size( AVPixelFormat( m_cStream->codecpar->format ), m_cStream->codecpar->width, m_cStream->codecpar->height, 1 );

  Int pix_fmt = m_cStream->codecpar->format;
  m_uiWidth = m_cStream->codecpar->width;
  m_uiHeight = m_cStream->codecpar->height;

  /**
   * Auxiliar conversation to re-use similar pixfmt
   */
  switch( pix_fmt )
  {
  case AV_PIX_FMT_YUVJ420P:
    pix_fmt = AV_PIX_FMT_YUV420P;
    break;
  case AV_PIX_FMT_YUVJ422P:
    pix_fmt = AV_PIX_FMT_YUV422P;
    break;
  case AV_PIX_FMT_YUVJ444P:
    pix_fmt = AV_PIX_FMT_YUV444P;
    break;
  case AV_PIX_FMT_GRAY16LE:
    m_uiBitsPerPixel = 16;
    m_iEndianness = 1;
    pix_fmt = AV_PIX_FMT_GRAY8;
    break;
  case AV_PIX_FMT_GRAY16BE:
    m_uiBitsPerPixel = 16;
    m_iEndianness = 0;
    pix_fmt = AV_PIX_FMT_GRAY8;
  }

  m_strFormatName = uppercase( strFilename.substr( strFilename.find_last_of( "." ) + 1 ) );
  const char* name = avcodec_get_name( m_cCodedCtx->codec_id );
  m_strCodecName = name;

  Double fr = 30;
  if( m_cStream->avg_frame_rate.den && m_cStream->avg_frame_rate.num )
    fr = av_q2d( m_cStream->avg_frame_rate );
#if FF_API_R_FRAME_RATE
  else if( m_cStream->r_frame_rate.den && m_cStream->r_frame_rate.num )
    fr = av_q2d( m_cStream->r_frame_rate );
#endif
  else if( m_cStream->time_base.den && m_cStream->time_base.num )
    fr = 1 / av_q2d( m_cStream->time_base );

  m_dFrameRate = fr;

  // Set bits per pixel to default (8 bits)
  m_uiBitsPerPixel = 8;

  m_iPixelFormat = PlaYUVerFrame::NO_FMT;
  for( Int i = 0; i < PlaYUVerFrame::NUMBER_PEL_FORMATS; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].ffmpegPelFormat == pix_fmt )
    {
      m_iPixelFormat = i;
      break;
    }
  }
  if( m_iPixelFormat == PlaYUVerFrame::NO_FMT )
  {
    throw PlaYUVerFailure( "Cannot open file using FFmpeg libs - unsupported pixel format" );
    return false;
  }

  /* dump input information to stderr */
  av_dump_format( m_cFmtCtx, 0, filename, 0 );
  if( !m_cStream )
  {
    std::cout << " Could not find audio or video stream in the input, aborting !!!" << std::endl;
    closeHandler();
    return false;
  }

  m_cFrame = av_frame_alloc();
  if( !m_cFrame )
  {
    closeHandler();
    return false;
  }

  /* initialize packet, set data to NULL, let the demuxer fill it */
  // m_pcPacket = av_packet_alloc();
  /* initialize packet, set data to NULL, let the demuxer fill it */
  av_init_packet( &m_cPacket );
  // m_cPacket.data = NULL;
  // m_cPacket.size = 0;

  m_bHasStream = true;
  return true;
}

Void StreamHandlerLibav::closeHandler()
{
  if( m_bHasStream )
  {
    if( m_cCodedCtx )
      avcodec_close( m_cCodedCtx );

    if( m_cFmtCtx )
      avformat_close_input( &m_cFmtCtx );

    av_free( m_cFrame );
    // av_packet_free( &m_pcPacket );
  }
  m_bHasStream = false;

  if( m_pStreamBuffer )
    freeMem1D( m_pStreamBuffer );
}

Bool StreamHandlerLibav::configureBuffer( PlaYUVerFrame* pcFrame )
{
  return getMem1D<Byte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}

UInt64 StreamHandlerLibav::calculateFrameNumber()
{
  UInt64 num_frames;
  //   if( m_cStream->nb_frames )
  //    {                                            *
  //    num_frames = m_cStream->nb_frames;
  //   }
  //   else
  if( m_cFmtCtx->duration != AV_NOPTS_VALUE )
  {
    Int64 duration = m_cFmtCtx->duration + 5000;
    m_uiSecs = duration / AV_TIME_BASE;
    num_frames = m_uiSecs * m_dFrameRate;
    m_uiMicroSec = duration % AV_TIME_BASE;
  }
  else
  {
    num_frames = 0;
  }
  num_frames = num_frames == 0 ? 1 : num_frames;
  return num_frames;
}

Bool StreamHandlerLibav::read( PlaYUVerFrame* pcFrame )
{
  Bool bGotFrame = false;
  Bool bErrors = false;
  Bool bReadPkt = false;
  Int iRet = 0;

  while( !bGotFrame && !bErrors )
  {
    if( ( iRet = avcodec_receive_frame( m_cCodedCtx, m_cFrame ) ) < 0 )
    {
      if( iRet == AVERROR( EAGAIN ) || iRet == AVERROR_EOF )
        bReadPkt = true;
      else
        return false;
    }
    else
    {
      bGotFrame = true;
      break;
    }
    if( bReadPkt )
    {
      bReadPkt = false;
      av_packet_unref( &m_cPacket );
      if( ( iRet = av_read_frame( m_cFmtCtx, &m_cPacket ) ) < 0 )
        return false;
      if( m_cPacket.stream_index == m_iStreamIdx )
        // iRet = avcodec_send_packet( m_cCodedCtx, &m_cPacket );
        if( ( iRet = avcodec_send_packet( m_cCodedCtx, &m_cPacket ) ) < 0 )
          return false;
    }
  }

  if( bGotFrame )
  {
    AVCodecParameters* codec_param = m_cStream->codecpar;
    av_image_copy_to_buffer( m_pStreamBuffer, m_uiFrameBufferSize, m_cFrame->data, m_cFrame->linesize, AVPixelFormat( codec_param->format ),
                             codec_param->width, codec_param->height, 1 );

    pcFrame->frameFromBuffer( m_pStreamBuffer, m_iEndianness );
    m_uiCurrFrameFileIdx++;
    return true;
  }
  return false;
}

Bool StreamHandlerLibav::write( PlaYUVerFrame* pcFrame )
{
  return false;
}

Bool StreamHandlerLibav::seek( UInt64 iFrameNum )
{
	if( m_uiCurrFrameFileIdx == iFrameNum )
		return true;

  Int flags = AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME;
  if( iFrameNum < m_uiCurrFrameFileIdx )
  {
    flags |= AVSEEK_FLAG_BACKWARD;
  }
  Int iRet = av_seek_frame( m_cFmtCtx, m_iStreamIdx, iFrameNum, flags );
  if( iRet < 0 )
  {
    return false;
  }
  m_uiCurrFrameFileIdx = iFrameNum;
  return true;
}
