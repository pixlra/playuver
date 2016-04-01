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
 * \file     LibAvContextHandle.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Interface with libav libs
 */

#include <cstdio>
#include "LibAvContextHandle.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"

namespace plaYUVer
{

std::vector<PlaYUVerSupportedFormat> LibAvContextHandle::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( "Portable Grayscale Map", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Joint Photographic Experts Group", "jpg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Windows Bitmap", "bmp" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Audio video interleaved", "avi" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Windows media video", "wmv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "MPEG4", "mp4" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Matroska Multimedia Container", "mkv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "H.264 streams", "h264" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "HEVC streams", "hevc" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> LibAvContextHandle::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> LibAvContextHandle::supportedSaveFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

static int open_codec_context( int *stream_idx, AVFormatContext *fmt_ctx, enum AVMediaType type )
{
  int ret;
  AVStream *st;
  AVCodecContext *dec_ctx = NULL;
  AVCodec *dec = NULL;

  ret = av_find_best_stream( fmt_ctx, type, -1, -1, NULL, 0 );
  if( ret < 0 )
  {
    return ret;
  }
  else
  {
    *stream_idx = ret;
    st = fmt_ctx->streams[*stream_idx];

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

Void LibAvContextHandle::closeAvFormat()
{
  if( m_bHasStream )
  {
    if( video_dec_ctx )
      avcodec_close( video_dec_ctx );

    if( fmt_ctx )
      avformat_close_input( &fmt_ctx );

    av_free( frame );
    //av_free( video_dst_data[0] );
  }
  m_bHasStream = false;
}

Bool LibAvContextHandle::initAvFormat( const char* filename, UInt& width, UInt& height, Int& pixel_format, UInt& bits_pel, Int& endianness, Double& frame_rate,
    UInt64& num_frames )
{
  fmt_ctx = NULL;
  video_dec_ctx = NULL;
  video_stream = NULL;
//  video_dst_data[0] = NULL;
//  video_dst_data[1] = NULL;
//  video_dst_data[2] = NULL;
//  video_dst_data[3] = NULL;
  video_stream_idx = -1;
  frame = NULL;
  m_bHasStream = false;

  AVDictionary *format_opts = NULL;

  /* register all formats and codecs */
  av_register_all();

  if( width > 0 && height > 0 )
  {
    Char aux_string[10];
    sprintf( aux_string, "%dx%d", width, height );
    av_dict_set( &format_opts, "video_size", aux_string, 0 );
  }

  Int ffmpeg_pel_format = g_PlaYUVerPixFmtDescriptorsList[pixel_format].ffmpegPelFormat;
  if( ffmpeg_pel_format >= 0 )
  {
    av_dict_set( &format_opts, "pixel_format", av_get_pix_fmt_name( AVPixelFormat( ffmpeg_pel_format ) ), 0 );
  }

  /* open input file, and allocate format context */
  if( avformat_open_input( &fmt_ctx, filename, NULL, &format_opts ) < 0 )
  {
    //qDebug( ) << " Could not open source file %s !!!" << filename << endl;
    return false;
  }

  /* retrieve stream information */
  if( avformat_find_stream_info( fmt_ctx, NULL ) < 0 )
  {
    //qDebug( ) << " Could not find stream information !!!" << endl;
    return false;
  }

  if( open_codec_context( &video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO ) >= 0 )
  {
    video_stream = fmt_ctx->streams[video_stream_idx];
    video_dec_ctx = video_stream->codec;

//    /* allocate image where the decoded image will be put */
//    ret = av_image_alloc( video_dst_data, video_dst_linesize, video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt, 1 );
//    if( ret < 0 )
//    {
//      //qDebug( ) << " Could not allocate raw video buffer !!!" << endl;
//      closeAvFormat();
//      return false;
//    }
//    video_dst_bufsize = ret;

    m_uiFrameBufferSize = av_image_get_buffer_size( video_dec_ctx->pix_fmt, video_dec_ctx->width, video_dec_ctx->height, 1 );
    getMem1D( &m_pchFrameBuffer, m_uiFrameBufferSize );
  }

  const char *name = avcodec_get_name( video_dec_ctx->codec_id );
  sprintf( m_acCodecName, "%s", name );

  Double fr = 30;
  if( video_stream->avg_frame_rate.den && video_stream->avg_frame_rate.num )
    fr = av_q2d( video_stream->avg_frame_rate );
#if FF_API_R_FRAME_RATE
  else if( video_stream->r_frame_rate.den && video_stream->r_frame_rate.num )
    fr = av_q2d( video_stream->r_frame_rate );
#endif
  else if( video_stream->time_base.den && video_stream->time_base.num )
    fr = 1 / av_q2d( video_stream->time_base );
  else if( video_stream->codec->time_base.den && video_stream->codec->time_base.num )
    fr = 1 / av_q2d( video_stream->codec->time_base );

  frame_rate = fr;

  /*if( video_stream->nb_frames )
   {
   num_frames = video_stream->nb_frames;
   }
   else*/if( fmt_ctx->duration != AV_NOPTS_VALUE )
  {
    Int64 duration = fmt_ctx->duration + 5000;
    m_uiSecs = duration / AV_TIME_BASE;
    num_frames = m_uiSecs * frame_rate;
    m_uiMicroSec = duration % AV_TIME_BASE;
  }
  else
  {
    num_frames = 0;
  }
  num_frames = num_frames == 0 ? 1 : num_frames;

  Int pix_fmt = video_dec_ctx->pix_fmt;

  // Set bits per pixel to default (8 bits)
  bits_pel = 8;

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
  case AV_PIX_FMT_GRAY16BE:
    bits_pel = 16;
    pix_fmt = AV_PIX_FMT_GRAY8;
    break;
  }

  width = video_dec_ctx->width;
  height = video_dec_ctx->height;

  pixel_format = PlaYUVerFrame::NO_FMT;
  for( Int i = 0; i < PLAYUVER_NUMBER_FORMATS; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].ffmpegPelFormat == pix_fmt )
    {
      pixel_format = i;
      break;
    }
  }
  if( pixel_format == PlaYUVerFrame::NO_FMT )
  {
    throw "Cannot open file using FFmpeg libs - unsupported pixel format";
    closeAvFormat();
    return false;
  }

  /* dump input information to stderr */
  av_dump_format( fmt_ctx, 0, filename, 0 );
  if( !video_stream )
  {
    //qDebug( ) << " Could not find audio or video stream in the input, aborting !!!" << endl;
    closeAvFormat();
    return false;
  }

#if( LIBAVFORMAT_VERSION_MAJOR >= 55 )
  frame = av_frame_alloc();
#else
  frame = avcodec_alloc_frame();
#endif
  if( !frame )
  {
    //qDebug( ) << " Could not allocate frame !!!" << endl;
    //ret = AVERROR( ENOMEM );
    closeAvFormat();
    return false;
  }

  /* initialize packet, set data to NULL, let the demuxer fill it */
  av_init_packet( &pkt );
  pkt.data = NULL;
  pkt.size = 0;

  m_bHasStream = true;

  return true;
}

Bool LibAvContextHandle::decodeVideoPkt()
{
  Int got_frame = 0;
  Int decResult;
  if( pkt.stream_index == video_stream_idx )
  {
    decResult = avcodec_decode_video2( video_dec_ctx, frame, &got_frame, &pkt );
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

Bool LibAvContextHandle::decodeAvFormat()
{
  Bool bGotFrame = false;
  AVPacket orig_pkt = pkt;

  bGotFrame |= decodeVideoPkt();

  /* read frames from the file */
  while( !bGotFrame && ( av_read_frame( fmt_ctx, &pkt ) >= 0 ) )
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
    av_image_copy_to_buffer( m_pchFrameBuffer, m_uiFrameBufferSize, frame->data, frame->linesize, video_dec_ctx->pix_fmt, video_dec_ctx->width,
        video_dec_ctx->height, 1 );

    if( orig_pkt.size )
      av_free_packet( &orig_pkt );

    return true;
  }

  return false;
}

Void LibAvContextHandle::seekAvFormat( UInt64 frame_num )
{
  av_seek_frame( fmt_ctx, video_stream_idx, frame_num, AVSEEK_FLAG_FRAME );
}

}  // NAMESPACE
