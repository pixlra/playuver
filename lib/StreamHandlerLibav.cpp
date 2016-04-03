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
 * \file     StreamHandlerLibav.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Interface with libav libs
 */

#include <cstdio>
#include "StreamHandlerLibav.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"

namespace plaYUVer
{

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Portable Grayscale Map", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Joint Photographic Experts Group", "jpg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows Bitmap", "bmp" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Audio video interleaved", "avi" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Windows media video", "wmv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "MPEG4", "mp4" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "Matroska Multimedia Container", "mkv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "H.264 streams", "h264" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerLibav::Create, "HEVC streams", "hevc" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> StreamHandlerLibav::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

static int open_codec_context( int *stream_idx, AVFormatContext *m_cFmtCtx, enum AVMediaType type )
{
  int ret;
  AVStream *st;
  AVCodecContext *dec_ctx = NULL;
  AVCodec *dec = NULL;

  ret = av_find_best_stream( m_cFmtCtx, type, -1, -1, NULL, 0 );
  if( ret < 0 )
  {
    return ret;
  }
  else
  {
    *stream_idx = ret;
    st = m_cFmtCtx->streams[*stream_idx];

    /* find decoder for the stream */
    dec_ctx = st->codec;
    dec = avcodec_find_decoder( dec_ctx->codec_id );
    if( !dec )
    {
      printf( "Failed to find %s codec\n", av_get_media_type_string( type ) );
      return ret;
    }

    if( ( ret = avcodec_open2( dec_ctx, dec, NULL ) ) < 0 )
    {
      printf( "Failed to open %s codec\n", av_get_media_type_string( type ) );
      return ret;
    }
  }

  return 0;
}

Bool StreamHandlerLibav::openHandler( std::string strFilename, Bool bInput )
{
  const char* filename = strFilename.c_str();

  m_cFmtCtx = NULL;
  m_cCodecCtx = NULL;
  m_cStream = NULL;
//  video_dst_data[0] = NULL;
//  video_dst_data[1] = NULL;
//  video_dst_data[2] = NULL;
//  video_dst_data[3] = NULL;
  m_iStreamIdx = -1;
  m_cFrame = NULL;
  m_bHasStream = false;

  AVDictionary *format_opts = NULL;

  /* register all formats and codecs */
  av_register_all();

//   if( width > 0 && height > 0 )
//   {
//     Char aux_string[10];
//     sprintf( aux_string, "%dx%d", width, height );
//     av_dict_set( &format_opts, "video_size", aux_string, 0 );
//   }

//   Int ffmpeg_pel_format = g_PlaYUVerPixFmtDescriptorsList[pixel_format].ffmpegPelFormat;
//   if( ffmpeg_pel_format >= 0 )
//   {
//     av_dict_set( &format_opts, "pixel_format", av_get_pix_fmt_name( AVPixelFormat( ffmpeg_pel_format ) ), 0 );
//   }

  /* open input file, and allocate format context */
  if( avformat_open_input( &m_cFmtCtx, filename, NULL, &format_opts ) < 0 )
  {
    //qDebug( ) << " Could not open source file %s !!!" << filename << endl;
    return false;
  }

  /* retrieve stream information */
  if( avformat_find_stream_info( m_cFmtCtx, NULL ) < 0 )
  {
    //qDebug( ) << " Could not find stream information !!!" << endl;
    return false;
  }

  if( open_codec_context( &m_iStreamIdx, m_cFmtCtx, AVMEDIA_TYPE_VIDEO ) >= 0 )
  {
    m_cStream = m_cFmtCtx->streams[m_iStreamIdx];
    m_cCodecCtx = m_cStream->codec;

//    /* allocate image where the decoded image will be put */
//    ret = av_image_alloc( video_dst_data, video_dst_linesize, m_cCodecCtx->width, m_cCodecCtx->height, m_cCodecCtx->pix_fmt, 1 );
//    if( ret < 0 )
//    {
//      //qDebug( ) << " Could not allocate raw video buffer !!!" << endl;
//      closeAvFormat();
//      return false;
//    }
//    video_dst_bufsize = ret;

    m_uiFrameBufferSize = av_image_get_buffer_size( m_cCodecCtx->pix_fmt, m_cCodecCtx->width, m_cCodecCtx->height, 1 );
    getMem1D( &m_pchFrameBuffer, m_uiFrameBufferSize );
  }

  m_strFormatName = uppercase (strFilename.substr( strFilename.find_last_of( "." ) + 1 ) );
  const char *name = avcodec_get_name( m_cCodecCtx->codec_id );
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
  else if( m_cStream->codec->time_base.den && m_cStream->codec->time_base.num )
    fr = 1 / av_q2d( m_cStream->codec->time_base );

  m_dFrameRate = fr;

  Int pix_fmt = m_cCodecCtx->pix_fmt;

  // Set bits per pixel to default (8 bits)
  m_uiBitsPerPixel = 8;

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

  m_uiWidth = m_cCodecCtx->width;
  m_uiHeight = m_cCodecCtx->height;

  m_iPixelFormat = PlaYUVerFrame::NO_FMT;
  for( Int i = 0; i < PLAYUVER_NUMBER_FORMATS; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].ffmpegPelFormat == pix_fmt )
    {
      m_iPixelFormat = i;
      break;
    }
  }
  if( m_iPixelFormat == PlaYUVerFrame::NO_FMT )
  {
    throw "Cannot open file using FFmpeg libs - unsupported pixel format";
    return false;
  }

  /* dump input information to stderr */
  av_dump_format( m_cFmtCtx, 0, filename, 0 );
  if( !m_cStream )
  {
    //qDebug( ) << " Could not find audio or video stream in the input, aborting !!!" << endl;
    closeHandler();
    return false;
  }

#if( LIBAVFORMAT_VERSION_MAJOR >= 55 )
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
  av_init_packet( &pkt );
  pkt.data = NULL;
  pkt.size = 0;

  m_bHasStream = true;

  return true;
}

Void StreamHandlerLibav::closeHandler()
{
  if( m_bHasStream )
  {
    if( m_cCodecCtx )
      avcodec_close( m_cCodecCtx );

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

Bool StreamHandlerLibav::decodeVideoPkt()
{
  Int got_frame = 0;
  Int decResult;
  if( pkt.stream_index == m_iStreamIdx )
  {
    decResult = avcodec_decode_video2( m_cCodecCtx, m_cFrame, &got_frame, &pkt );
    if( decResult < 0 )
    {
      fprintf( stderr, "Error decoding video frame\n" );
      return false;
    }
    pkt.data += decResult;
    pkt.size -= decResult;
  }
  else
  {
    pkt.size = 0;
  }
  return got_frame > 0 ? true : false;
}

Bool StreamHandlerLibav::read( PlaYUVerFrame* pcFrame )
{
  Bool bGotFrame = false;
  AVPacket orig_pkt = pkt;

  bGotFrame |= decodeVideoPkt();

  /* read frames from the file */
  while( !bGotFrame && ( av_read_frame( m_cFmtCtx, &pkt ) >= 0 ) )
  {
    orig_pkt = pkt;
    do
    {
      bGotFrame |= decodeVideoPkt();
      if( bGotFrame )
        break;
    }
    while( pkt.size > 0 );

    if( bGotFrame )
    {
      break;
    }
  }

  /*! Try flush existing packets */
  if( !bGotFrame )
  {
    bGotFrame |= decodeVideoPkt();
  }

  if( bGotFrame )
  {
//     av_image_copy_to_buffer( m_pchFrameBuffer, m_uiFrameBufferSize, m_cFrame->data, m_cFrame->linesize, m_cCodecCtx->pix_fmt, m_cCodecCtx->width,
//                              m_cCodecCtx->height, 1 );
    av_image_copy_to_buffer( m_pStreamBuffer, m_uiFrameBufferSize, m_cFrame->data, m_cFrame->linesize, m_cCodecCtx->pix_fmt, m_cCodecCtx->width,
                             m_cCodecCtx->height, 1 );
    if( orig_pkt.size )
      av_free_packet( &orig_pkt );

    pcFrame->frameFromBuffer( m_pStreamBuffer );

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

}  // NAMESPACE
