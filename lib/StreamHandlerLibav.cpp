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
 * \file     StreamHandlerLibav.cpp
 * \brief    interface with libav libs
 */

#include "StreamHandlerLibav.h"

#include "CalypFrame.h"
#include "LibMemory.h"
#include "PixelFormats.h"

#include <cstdio>
#include <iostream>

#if( ( LIBAVCODEC_VERSION_MAJOR >= 57 ) && ( LIBAVCODEC_VERSION_MINOR >= 37 ) )
#define FF_SEND_RECEIVE_API
#define FF_USER_CODEC_PARAM
#endif

std::vector<CalypStreamFormat> StreamHandlerLibav::supportedReadFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Portable Network Graphics", "png" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Joint Photographic Experts Group", "jpg,jpeg" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows Bitmap", "bmp" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Tagged Image File Format", "tiff" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Graphics interchange Format", "gif" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Audio video interleaved", "avi" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows media video", "wmv" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "MPEG-4", "mp4" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Matroska Multimedia Container", "mkv" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "H.264 streams", "264,h264" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerLibav::Create, "HEVC streams", "265,hevc" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStreamFormat> StreamHandlerLibav::supportedWriteFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  END_REGIST_CALYP_SUPPORTED_FMT;
}

StreamHandlerLibav::StreamHandlerLibav()
{
  m_pchHandlerName = "FFmpeg";
}

bool StreamHandlerLibav::openHandler( ClpString strFilename, bool bInput )
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
  //	int ffmpeg_pel_format =
  // g_CalypPixFmtDescriptorsList[m_iPixelFormat].ffmpegPelFormat; 	if(
  // ffmpeg_pel_format >= 0 )
  //	{
  //		av_dict_set( &format_opts, "pix_fmt", av_get_pix_fmt_name(
  // AVPixelFormat( ffmpeg_pel_format ) ), 0 );
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
#ifdef FF_USER_CODEC_PARAM
  int codecId = m_cStream->codecpar->codec_id;
#else
  int codecId = m_cStream->codec->codec_id;
#endif
  AVCodec* dec = avcodec_find_decoder( AVCodecID( codecId ) );
  if( !dec )
  {
    std::cout << "Failed to find video coded" << std::endl;
    return false;
  }

#ifdef FF_API_LAVF_AVCTX
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
#else
  m_cCodedCtx = m_cStream->codec;
#endif

  if( avcodec_open2( m_cCodedCtx, dec, NULL ) < 0 )
  {
    std::cout << "Failed to open video coded" << std::endl;
    return false;
  }

#ifdef FF_USER_CODEC_PARAM
  m_ffPixFmt = m_cStream->codecpar->format;
  m_uiWidth = m_cStream->codecpar->width;
  m_uiHeight = m_cStream->codecpar->height;
#else
  m_ffPixFmt = m_cStream->codec->pix_fmt;
  m_uiWidth = m_cStream->codec->width;
  m_uiHeight = m_cStream->codec->height;

#endif
  m_strFormatName = clpUppercase( strFilename.substr( strFilename.find_last_of( "." ) + 1 ) );
  const char* name = avcodec_get_name( m_cCodedCtx->codec_id );
  m_strCodecName = name;

  // Set bits per pixel to default (8 bits)
  m_uiBitsPerPixel = 8;

  /**
	 * Auxiliar conversation to re-use similar pixfmt
	 */
  int auxPixFmt = m_ffPixFmt;
  switch( m_ffPixFmt )
  {
  case AV_PIX_FMT_YUVJ420P:
    auxPixFmt = AV_PIX_FMT_YUV420P;
    break;
  case AV_PIX_FMT_YUVJ422P:
    auxPixFmt = AV_PIX_FMT_YUV422P;
    break;
  case AV_PIX_FMT_YUVJ444P:
    auxPixFmt = AV_PIX_FMT_YUV444P;
    break;
  case AV_PIX_FMT_GRAY16LE:
    m_uiBitsPerPixel = 16;
    m_iEndianness = 1;
    auxPixFmt = AV_PIX_FMT_GRAY8;
    break;
  case AV_PIX_FMT_GRAY16BE:
    m_uiBitsPerPixel = 16;
    m_iEndianness = 0;
    auxPixFmt = AV_PIX_FMT_GRAY8;
  }

  m_iPixelFormat = CLP_INVALID_FMT;
  for( int i = 0; i < CalypFrame::numberOfFormats(); i++ )
  {
    if( g_CalypPixFmtDescriptorsMap.at( i ).ffmpegPelFormat == auxPixFmt )
    {
      m_iPixelFormat = i;
      break;
    }
  }
  if( m_iPixelFormat == CLP_INVALID_FMT )
  {
    m_bNative = false;
    //throw CalypFailure( "StreamHandlerLibav", "Cannot open file using FFmpeg libs - unsupported pixel format" );
    //return false;
  }

  double fr = 30;
  if( m_cStream->avg_frame_rate.den && m_cStream->avg_frame_rate.num )
    fr = av_q2d( m_cStream->avg_frame_rate );
#if FF_API_R_FRAME_RATE
  else if( m_cStream->r_frame_rate.den && m_cStream->r_frame_rate.num )
    fr = av_q2d( m_cStream->r_frame_rate );
#endif
  else if( m_cStream->time_base.den && m_cStream->time_base.num )
    fr = 1 / av_q2d( m_cStream->time_base );
#ifndef FF_USER_CODEC_PARAM
  else if( m_cStream->codec->time_base.den && m_cStream->codec->time_base.num )
    fr = 1 / av_q2d( m_cStream->codec->time_base );
#endif

  m_dFrameRate = fr;

  calculateFrameNumber();

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

  if( !m_bNative )
  {
    int newPelFmt = -1;
    const AVPixFmtDescriptor* ffPelDesc = av_pix_fmt_desc_get( AVPixelFormat( m_ffPixFmt ) );
    if( ffPelDesc->flags & AV_PIX_FMT_FLAG_PAL )
    {
      newPelFmt = CLP_RGB24;
    }
    else if( ffPelDesc->flags & AV_PIX_FMT_FLAG_ALPHA )
    {
      newPelFmt = CLP_RGBA32;
    }
    else if( ffPelDesc->flags & AV_PIX_FMT_FLAG_RGB )
    {
      newPelFmt = CLP_RGB24;
    }
    else if( ffPelDesc->nb_components == 1 )
    {
      newPelFmt = CLP_GRAY;
    }
    else
    {
      newPelFmt = CLP_YUV444P;
    }
    m_iPixelFormat = newPelFmt;

    AVPixelFormat newAvFmt = AVPixelFormat( g_CalypPixFmtDescriptorsMap.at( m_iPixelFormat ).ffmpegPelFormat );

    /* create scaling context */
    m_ScalerCtx = sws_getContext( m_uiWidth, m_uiHeight, AVPixelFormat( m_ffPixFmt ),
                                  m_uiWidth, m_uiHeight, newAvFmt,
                                  SWS_BILINEAR, NULL, NULL, NULL );

    m_cConvertedFrame = av_frame_alloc();
    if( !m_cConvertedFrame )
    {
      closeHandler();
      return false;
    }

    if( av_image_alloc( m_cConvertedFrame->data, m_cConvertedFrame->linesize,
                        m_uiWidth, m_uiHeight, newAvFmt, 16 ) < 0 )
    {
      closeHandler();
      return false;
    }

    m_ffPixFmt = newAvFmt;
  }

  m_uiFrameBufferSize = av_image_get_buffer_size( AVPixelFormat( m_ffPixFmt ), m_uiWidth, m_uiHeight, 1 );

  /* initialize packet, set data to NULL, let the demuxer fill it */
  av_init_packet( &m_cPacket );
  m_cPacket.data = NULL;
  m_cPacket.size = 0;
  m_cOrgPacket = m_cPacket;

  m_bHasStream = true;
  return true;
}

void StreamHandlerLibav::closeHandler()
{
  if( m_bHasStream )
  {
    if( m_cCodedCtx )
      avcodec_close( m_cCodedCtx );

    if( m_cFmtCtx )
      avformat_close_input( &m_cFmtCtx );

    av_free( m_cFrame );
  }
  m_bHasStream = false;

  if( m_pStreamBuffer )
    freeMem1D( m_pStreamBuffer );
}

bool StreamHandlerLibav::configureBuffer( CalypFrame* pcFrame )
{
  return getMem1D<ClpByte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}

void StreamHandlerLibav::calculateFrameNumber()
{
  unsigned long long int num_frames;
  if( m_cStream->nb_frames )
  {
    num_frames = m_cStream->nb_frames;
  }
  else if( m_cFmtCtx->duration != AV_NOPTS_VALUE )
  {
    long long int duration = m_cFmtCtx->duration + 5000;
    m_uiSecs = duration / AV_TIME_BASE;
    num_frames = m_uiSecs * m_dFrameRate;
    m_uiMicroSec = duration % AV_TIME_BASE;
  }
  else
  {
    num_frames = 0;
  }
  num_frames = num_frames == 0 ? 1 : num_frames;
  m_uiTotalNumberFrames = num_frames;
}

bool StreamHandlerLibav::read( CalypFrame* pcFrame )
{
  int bGotFrame = 0;
  bool bErrors = false;
  bool bReadPkt = false;
  int iRet = 0;

  while( !bGotFrame && !bErrors )
  {
#ifdef FF_SEND_RECEIVE_API
    if( ( iRet = avcodec_receive_frame( m_cCodedCtx, m_cFrame ) ) < 0 )
    {
      if( iRet == AVERROR( EAGAIN ) || iRet == AVERROR_EOF )
        bReadPkt = true;
      else
        return false;
    }
    else
    {
      bGotFrame = 1;
    }
#else
    if( m_cPacket.stream_index == m_iStreamIdx )
    {
      if( ( iRet = avcodec_decode_video2( m_cCodedCtx, m_cFrame, &bGotFrame, &m_cPacket ) ) < 0 )
        return false;
      m_cPacket.data += iRet;
      m_cPacket.size -= iRet;
      if( m_cPacket.size <= 0 )
      {
        bReadPkt = true;
        av_packet_unref( &m_cOrgPacket );
      }
    }
#endif
    if( bGotFrame )
      break;
    if( bReadPkt )
    {
      bReadPkt = false;
      av_packet_unref( &m_cOrgPacket );
      if( ( iRet = av_read_frame( m_cFmtCtx, &m_cPacket ) ) < 0 )
        return false;
      m_cOrgPacket = m_cPacket;
#ifdef FF_SEND_RECEIVE_API
      if( m_cPacket.stream_index == m_iStreamIdx )
        if( ( iRet = avcodec_send_packet( m_cCodedCtx, &m_cPacket ) ) < 0 )
          return false;
#endif
    }
  }

  if( bGotFrame )
  {
    AVFrame* decFrame = m_cFrame;
    if( !m_bNative )
    {
      sws_scale( m_ScalerCtx, (const uint8_t* const*)decFrame->data, decFrame->linesize, 0,
                 decFrame->height, (uint8_t* const*)m_cConvertedFrame->data, m_cConvertedFrame->linesize );

      decFrame = m_cConvertedFrame;
    }
    av_image_copy_to_buffer( m_pStreamBuffer, m_uiFrameBufferSize, decFrame->data,
                             decFrame->linesize, AVPixelFormat( m_ffPixFmt ), m_uiWidth, m_uiHeight, 1 );

    pcFrame->frameFromBuffer( m_pStreamBuffer, m_iEndianness );
    m_uiCurrFrameFileIdx++;
    return true;
  }
  return false;
}

bool StreamHandlerLibav::write( CalypFrame* pcFrame )
{
  return false;
}

bool StreamHandlerLibav::seek( unsigned long long int iFrameNum )
{
  if( m_uiTotalNumberFrames == 1 )
    return true;

  if( m_uiCurrFrameFileIdx == iFrameNum )
    return true;

  int flags = AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME;
  if( iFrameNum < m_uiCurrFrameFileIdx )
  {
    flags |= AVSEEK_FLAG_BACKWARD;
  }
  int iRet = av_seek_frame( m_cFmtCtx, m_iStreamIdx, iFrameNum, flags );
  if( iRet < 0 )
  {
    return false;
  }
  m_uiCurrFrameFileIdx = iFrameNum;
  return true;
}
