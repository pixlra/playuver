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
 * \file     LibAvContextHandle.cpp
 * \brief    Interface with libav libs
 */

#include "LibAvContextHandle.h"

#ifdef USE_FFMPEG

#include <cstdio>

#include <QtDebug>

#include "LibMemAlloc.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

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
      av_free( video_dst_data[0] );
    }
    m_bHasStream = false;
  }

  Bool LibAvContextHandle::initAvFormat( QString filename, UInt& width, UInt& height, Int& pixel_format, UInt& frame_rate )
  {
    Bool bRet = true;
    int ret = 0;

    fmt_ctx = NULL;
    video_dec_ctx = NULL;
    video_stream = NULL;
    video_dst_data[0] = NULL;
    video_dst_data[1] = NULL;
    video_dst_data[2] = NULL;
    video_dst_data[3] = NULL;
    video_stream_idx = -1;
    frame = NULL;
    m_bHasStream = false;

    char *src_filename = filename.toLocal8Bit().data();
    AVDictionary *format_opts = NULL;

    /* register all formats and codecs */
    av_register_all();

    if( width >= 0 && height >= 0 )
    {
      Char aux_string[10];
      sprintf( aux_string, "%dx%d", width, height );
      av_dict_set( &format_opts, "video_size", aux_string, 0 );
    }
    switch( pixel_format )
    {
      case PlaYUVerFrame::NO_FMT:
      break;
      case PlaYUVerFrame::YUV420:
      av_dict_set( &format_opts, "pixel_format", av_get_pix_fmt_name( AV_PIX_FMT_YUV420P ), 0 );
      break;
      case PlaYUVerFrame::YUV400:
      av_dict_set( &format_opts, "pixel_format", av_get_pix_fmt_name( AV_PIX_FMT_GRAY8 ), 0 );
      break;
    }

    /* open input file, and allocate format context */
    if( avformat_open_input( &fmt_ctx, src_filename, NULL, &format_opts ) < 0 )
    {
      qDebug( ) << " Could not open source file %s !!!" << filename << endl;
      return false;
    }

    /* retrieve stream information */
    if( avformat_find_stream_info( fmt_ctx, NULL ) < 0 )
    {
      qDebug( ) << " Could not find stream information !!!" << endl;
      return false;
    }

    if( open_codec_context( &video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO ) >= 0 )
    {
      video_stream = fmt_ctx->streams[video_stream_idx];
      video_dec_ctx = video_stream->codec;

      /* allocate image where the decoded image will be put */
      ret = av_image_alloc( video_dst_data, video_dst_linesize, video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt, 1 );
      if( ret < 0 )
      {
        qDebug( ) << " Could not allocate raw video buffer !!!" << endl;
        closeAvFormat();
        return false;
      }
      video_dst_bufsize = ret;
    }

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

    frame_rate = ( UInt )fr;

    width = video_dec_ctx->width;
    height = video_dec_ctx->height;
    switch( video_dec_ctx->pix_fmt )
    {
      case AV_PIX_FMT_YUV420P:
      pixel_format = PlaYUVerFrame::YUV420;
      break;
      case AV_PIX_FMT_GRAY8:
      pixel_format = PlaYUVerFrame::YUV400;
      break;
    }

    /* dump input information to stderr */
    av_dump_format( fmt_ctx, 0, src_filename, 0 );
    if( !video_stream )
    {
      qDebug( ) << " Could not find audio or video stream in the input, aborting !!!" << endl;
      bRet = false;
      closeAvFormat();
      return false;
    }

    frame = avcodec_alloc_frame();
    if( !frame )
    {
      qDebug( ) << " Could not allocate frame !!!" << endl;
      ret = AVERROR( ENOMEM );
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

  Bool LibAvContextHandle::decodeAvFormat()
  {
    Int got_frame;
    int ret;
    /* read frames from the file */

    if( av_read_frame( fmt_ctx, &pkt ) >= 0 )
    {
      if( pkt.stream_index == video_stream_idx )
      {
        /* decode video frame */
        ret = avcodec_decode_video2( video_dec_ctx, frame, &got_frame, &pkt );
        if( ret < 0 )
        {
          fprintf( stderr, "Error decoding video frame\n" );
          return false;
        }

        if( got_frame )
        {
          /* copy decoded frame to destination buffer:
           * this is required since rawvideo expects non aligned data */
          av_image_copy( video_dst_data, video_dst_linesize, ( const uint8_t ** )( frame->data ), frame->linesize, video_dec_ctx->pix_fmt, video_dec_ctx->width,
              video_dec_ctx->height );

          /* write to rawvideo file */
          //fwrite( video_dst_data[0], 1, video_dst_bufsize, video_dst_file );
        }
      }
      av_free_packet( &pkt );
    }

//  /* flush cached frames */
//  pkt.data = NULL;
//  pkt.size = 0;
//  do
//  {
//    decode_packet( &got_frame, 1 );
//  }
//  while( got_frame );

    true;
  }

  Void LibAvContextHandle::seekAvFormat( UInt frame_num )
  {
    av_seek_frame( fmt_ctx, video_stream_idx, frame_num, AVSEEK_FLAG_ANY );
  }

}  // NAMESPACE

#endif
