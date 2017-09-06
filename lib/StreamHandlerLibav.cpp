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

#if( ( LIBAVCODEC_VERSION_MAJOR >= 57 ) && ( LIBAVCODEC_VERSION_MINOR >= 24 ) )
#define FF_USER_CODEC_PARAM
#endif

#if( ( LIBAVCODEC_VERSION_MAJOR >= 57 ) && ( LIBAVCODEC_VERSION_MINOR >= 37 ) )
#define FF_SEND_RECEIVE_API
#endif

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Joint Photographic Experts Group",
                                 "jpg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows Bitmap", "bmp" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Tagged Image File Format", "tiff" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Audio video interleaved", "avi" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows media video", "wmv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "MPEG4", "mp4" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Matroska Multimedia Container",
                                 "mkv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "H.264 streams", "h264" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "HEVC streams", "hevc" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

Bool StreamHandlerLibav::openHandler( String strFilename, Bool bInput )
{
  const char* filename = strFilename.c_str();

  m_cFmtCtx = NULL;
  m_cStream = NULL;

  //  video_dst_data[0] = NULL;
  //  video_dst_data[1] = NULL;
  //  video_dst_data[2] = NULL;
  //  video_dst_data[3] = NULL;
  m_iStreamIdx = -1;
  m_cFrame = NULL;
  m_bHasStream = false;

  AVDictionary* format_opts = NULL;

  /* register all formats and codecs */
  av_register_all();

  //   if( width > 0 && height > 0 )
  //   {
  //     Char aux_string[10];
  //     sprintf( aux_string, "%dx%d", width, height );
  //     av_dict_set( &format_opts, "video_size", aux_string, 0 );
  //   }

  //   Int ffmpeg_pel_format =
  //   g_PlaYUVerPixFmtDescriptorsList[pixel_format].ffmpegPelFormat;
  //   if( ffmpeg_pel_format >= 0 )
  //   {
  //     av_dict_set( &format_opts, "pixel_format", av_get_pix_fmt_name(
  //     AVPixelFormat( ffmpeg_pel_format ) ), 0 );
  //   }

  /* open input file, and allocate format context */
  if( avformat_open_input( &m_cFmtCtx, filename, NULL, &format_opts ) < 0 )
  {
    // qDebug( ) << " Could not open source file %s !!!" << filename << endl;
    return false;
  }

  /* retrieve stream information */
  if( avformat_find_stream_info( m_cFmtCtx, NULL ) < 0 )
  {
    // qDebug( ) << " Could not find stream information !!!" << endl;
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
  AVCodecParameters* codec_param = m_cStream->codecpar;
  AVCodec* dec = avcodec_find_decoder( codec_param->codec_id );
#else
  AVCodec* dec = avcodec_find_decoder( m_cStream->codec->codec_id );
#endif

  if( !dec )
  {
    printf( "Failed to find %s codec\n", av_get_media_type_string( AVMEDIA_TYPE_VIDEO ) );
    return false;
  }

  m_cCodedCtx = avcodec_alloc_context3( dec );

  if( avcodec_open2( m_cCodedCtx, dec, NULL ) < 0 )
  {
    printf( "Failed to open %s codec\n", av_get_media_type_string( AVMEDIA_TYPE_VIDEO ) );
    return false;
  }

#ifdef FF_USER_CODEC_PARAM
  m_uiFrameBufferSize = av_image_get_buffer_size( AVPixelFormat( codec_param->format ),
                                                  codec_param->width, codec_param->height, 1 );

  Int pix_fmt = codec_param->format;
  m_uiWidth = codec_param->width;
  m_uiHeight = codec_param->height;
#else
  m_uiFrameBufferSize = av_image_get_buffer_size(
      m_cStream->codec->pix_fmt, m_cStream->codec->width, m_cStream->codec->height, 1 );

  Int pix_fmt = m_cStream->codec->pix_fmt;
  m_uiWidth = m_cStream->codec->width;
  m_uiHeight = m_cStream->codec->height;

#endif

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
  //   sprintf( m_acCodecName, "%s", name );
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
#ifndef FF_USER_CODEC_PARAM
  else if( m_cStream->codec->time_base.den && m_cStream->codec->time_base.num )
    fr = 1 / av_q2d( m_cStream->codec->time_base );
#endif

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
    // qDebug( ) << " Could not find audio or video stream in the input,
    // aborting !!!" << endl;
    closeHandler();
    return false;
  }

#if( ( LIBAVCODEC_VERSION_MAJOR >= 56 ) && ( LIBAVCODEC_VERSION_MINOR >= 0 ) )
	m_cFrame = av_frame_alloc();
#else
	m_cFrame = avcodec_alloc_frame();
#endif
  if( !m_cFrame )
  {
    closeHandler();
    return false;
  }

  /* initialize packet, set data to NULL, let the demuxer fill it */
  av_init_packet( &m_cPacket );
  m_cPacket.data = NULL;
  m_cPacket.size = 0;

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

Int StreamHandlerLibav::decodeVideoPkt( Bool& gotFrame )
{
  gotFrame = false;

  Int decResult = 0;
  if( m_cPacket.stream_index == m_iStreamIdx )
  {
#ifdef FF_SEND_RECEIVE_API
    decResult = avcodec_receive_frame( m_cCodedCtx, m_cFrame );
    if( decResult < 0 && decResult != AVERROR( EAGAIN ) && decResult != AVERROR_EOF )
    {
      fprintf( stderr, "Error decoding video frame\n" );
      return decResult;
    }
    if( decResult >= 0 )
      gotFrame = true;
#else
    Int got_frame = false;
    decResult = avcodec_decode_video2( m_cCodedCtx, m_cFrame, &got_frame, &m_cPacket );
    if( decResult < 0 )
    {
      fprintf( stderr, "Error decoding video frame\n" );
      return decResult;
    }
    gotFrame = got_frame > 0 ? true : false;
#endif
    m_cPacket.data += decResult;
    m_cPacket.size -= decResult;
  }
  else
  {
    m_cPacket.size = 0;
  }
  return decResult;
}

Bool StreamHandlerLibav::read( PlaYUVerFrame* pcFrame )
{
  Bool bGotFrame = false;
  AVPacket orig_pkt = m_cPacket;

  decodeVideoPkt( bGotFrame );

  /* read frames from the file */
  while( !bGotFrame && ( av_read_frame( m_cFmtCtx, &m_cPacket ) >= 0 ) )
  {
    orig_pkt = m_cPacket;
#ifdef FF_SEND_RECEIVE_API
    avcodec_send_packet( m_cCodedCtx, &m_cPacket );
#endif
    do
    {
      decodeVideoPkt( bGotFrame );
      if( bGotFrame )
        break;
    } while( m_cPacket.size > 0 );

    if( bGotFrame )
    {
      break;
    }
  }

  /*! Try flush existing packets */
  if( !bGotFrame )
  {
    decodeVideoPkt( bGotFrame );
  }

  if( bGotFrame )
  {
#ifdef FF_USER_CODEC_PARAM
    AVCodecParameters* codec_param = m_cStream->codecpar;
    av_image_copy_to_buffer( m_pStreamBuffer, m_uiFrameBufferSize, m_cFrame->data,
                             m_cFrame->linesize, AVPixelFormat( codec_param->format ),
                             codec_param->width, codec_param->height, 1 );
#else
    av_image_copy_to_buffer( m_pStreamBuffer, m_uiFrameBufferSize, m_cFrame->data,
                             m_cFrame->linesize, m_cCodedCtx->pix_fmt, m_cCodedCtx->width,
                             m_cCodedCtx->height, 1 );

#endif

#ifndef FF_SEND_RECEIVE_API
    if( orig_pkt.size )
      av_free_packet( &orig_pkt );
#endif

    pcFrame->frameFromBuffer( m_pStreamBuffer, m_iEndianness );
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
  av_seek_frame( m_cFmtCtx, m_iStreamIdx, iFrameNum, AVSEEK_FLAG_FRAME );
  return true;
}
