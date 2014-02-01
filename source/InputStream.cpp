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
 * \file     InputStream.cpp
 * \brief    Input handling
 */

#include <cstdio>

#include <QtDebug>

#include "InputStream.h"
#include "LibMemAlloc.h"

namespace plaYUVer
{

InputStream::InputStream()
{
  m_iStatus = 0;

  m_pFile = NULL;
  m_uiWidth = 0;
  m_uiHeight = 0;
  m_uiTotalFrameNum = 0;
  m_uiCurrFrameNum = 0;
  m_iErrorStatus = 0;
  m_iPixelFormat = -1;
  m_pppcInputPel[0] = NULL;
  m_pppcInputPel[1] = NULL;
  m_pppcInputPel[2] = NULL;
  m_pppcRGBPel = NULL;

  m_iFileFormat = INVALID;
}

InputStream::~InputStream()
{
  if( m_pFile )
    fclose( m_pFile );

  if( m_pppcInputPel[0] )
    free_mem2Dpel( m_pppcInputPel[0] );
  if( m_pppcInputPel[1] )
    free_mem2Dpel( m_pppcInputPel[1] );
  if( m_pppcInputPel[2] )
    free_mem2Dpel( m_pppcInputPel[2] );

  if( m_pppcRGBPel )
    free_mem3Dpel( m_pppcRGBPel );

#ifdef USE_FFMPEG
  m_cLibAvContext.closeAvFormat();
#endif

  m_iStatus = 0;
}

QString InputStream::supportedReadFormats()
{
  QString formats;
  formats = "*.yuv "   // Raw video
          "*.avi"// Audio video interleaved
  ;
  return formats;
}

QString InputStream::supportedWriteFormats()
{
  QString formats;
  formats = "*.bmp "   // Windows Bitmap
          "*.jpg "// Joint Photographic Experts Group
          "*.jpeg "// Joint Photographic Experts Group
          "*.png "// Portable Network Graphics
  ;

  return formats;
}

QStringList InputStream::supportedReadFormatsList()
{
  QStringList formats;
  formats << "Raw video (*.yuv)"  // Raw video
      << "Audio video interleaved (*.avi)"  // Audio video interleaved
      ;

  return formats;
}

QStringList InputStream::supportedWriteFormatsList()
{
  QStringList formats;
  formats << "Windows Bitmap (*.bmp)"  // Windows Bitmap
      << "Joint Photographic Experts Group (*.jpg *.jpeg)"  // Joint Photographic Experts Group
      << "Portable Network Graphics (*.png)"  // Portable Network Graphics
      ;

  return formats;
}

QStringList InputStream::supportedPixelFormatList()
{
  QStringList formats;
  formats << "YUV420" << "YUV400";
  return formats;
}

Bool InputStream::needFormatDialog( QString filename )
{
  return true;
  QString fileExtension = QFileInfo( filename ).completeSuffix();
  if( !fileExtension.compare( QString( "yuv" ) ) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

#ifdef USE_FFMPEG
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
  if( video_dec_ctx )
    avcodec_close( video_dec_ctx );

  avformat_close_input( &fmt_ctx );

  av_free( frame );
  av_free( video_dst_data[0] );
}

Bool LibAvContextHandle::initAvFormat( QString filename, UInt width, UInt height )
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

  FILE *video_dst_file = NULL;

  /* register all formats and codecs */
  av_register_all();

  /* open input file, and allocate format context */
  if( avformat_open_input( &fmt_ctx, src_filename, NULL, NULL ) < 0 )
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
#endif

Void InputStream::init( QString filename, UInt width, UInt height, Int input_format )
{

  m_uiWidth = width;
  m_uiHeight = height;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

#ifdef USE_FFMPEG
  Bool avStatus = m_cLibAvContext.initAvFormat( filename, width, height );
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 )
  {
    //Error
    return;
  }

  m_cFilename = filename;
  m_pFile = fopen( m_cFilename.toLocal8Bit().data(), "rb" );
  if( m_pFile == NULL )
  {
    // Error
    return;
  }

  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;
  UInt64 alloc_memory;

  fseek( m_pFile, 0, SEEK_END );
  m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
  fseek( m_pFile, 0, SEEK_SET );

  if( m_iFileFormat == YUVFormat )
  {
    switch( m_iPixelFormat )
    {
    case YUV420:
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[1] ), m_uiHeight / 2, m_uiWidth / 2 );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[2] ), m_uiHeight / 2, m_uiWidth / 2 );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      break;
    case YUV400:
      alloc_memory = get_mem2Dpel( &( m_pppcInputPel[0] ), m_uiHeight, m_uiWidth );
      if( !alloc_memory )
      {
        //Error
        return;
      }
      break;
    }
  }

  get_mem3Dpel( &m_pppcRGBPel, 3, m_uiHeight, m_uiWidth );
  if( !alloc_memory )
  {
    //Error
    return;
  }

  m_iStatus = 1;

  return;
}

static inline void yuvToRgb( int iY, int iU, int iV, int &iR, int &iG, int &iB )
{
  iR = iY + 1402 * iV / 1000;
  iG = iY - ( 101004 * iU + 209599 * iV ) / 293500;
  iB = iY + 1772 * iU / 1000;

  if( iR < 0 )
    iR = 0;
  if( iG < 0 )
    iG = 0;
  if( iB < 0 )
    iB = 0;

  if( iR > 255 )
    iR = 255;
  if( iG > 255 )
    iG = 255;
  if( iB > 255 )
    iB = 255;
}

Void InputStream::YUV420toRGB()
{
  Int iY, iU, iV, iR, iG, iB;
  for( Int y = 0; y < m_uiHeight; y += 2 )
  {
    for( Int x = 0; x < m_uiWidth; x += 2 )
    {
      // Pixel (x, y).

      iY = m_pppcInputPel[0][y][x];
      iU = m_pppcInputPel[1][y >> 1][x >> 1];
      iV = m_pppcInputPel[2][y >> 1][x >> 1];
      iU -= 128;
      iV -= 128;

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y][x] = iR;
      m_pppcRGBPel[1][y][x] = iG;
      m_pppcRGBPel[2][y][x] = iB;

      // Pixel (x+1, y)

      iY = m_pppcInputPel[0][y][x + 1];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y][x + 1] = iR;
      m_pppcRGBPel[1][y][x + 1] = iG;
      m_pppcRGBPel[2][y][x + 1] = iB;

      // Pixel (x, y+1)

      iY = m_pppcInputPel[0][y + 1][x];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y + 1][x] = iR;
      m_pppcRGBPel[1][y + 1][x] = iG;
      m_pppcRGBPel[2][y + 1][x] = iB;

      // Pixel (x+1, y+1)

      iY = m_pppcInputPel[0][y + 1][x + 1];

      yuvToRgb( iY, iU, iV, iR, iG, iB );

      m_pppcRGBPel[0][y + 1][x + 1] = iR;
      m_pppcRGBPel[1][y + 1][x + 1] = iG;
      m_pppcRGBPel[2][y + 1][x + 1] = iB;

    }
  }
}

Void InputStream::readFrame()
{
  UInt64 bytes_read = 0;

  if( m_iStatus == 0 )
  {
    return;
  }
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;

#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    m_cLibAvContext.decodeAvFormat();
    //fwrite( video_dst_data[0], 1, video_dst_bufsize, video_dst_file );
    memcpy( &( m_pppcInputPel[0][0][0] ), m_cLibAvContext.video_dst_data[0], m_uiWidth * m_uiHeight * sizeof(uint8_t) );
    memcpy( &( m_pppcInputPel[1][0][0] ), m_cLibAvContext.video_dst_data[1], m_uiWidth * m_uiHeight * sizeof(uint8_t) / 4 );
    memcpy( &( m_pppcInputPel[2][0][0] ), m_cLibAvContext.video_dst_data[2], m_uiWidth * m_uiHeight * sizeof(uint8_t) / 4 );
    YUV420toRGB();
    return;
  }
#endif

  switch( m_iPixelFormat )
  {
  case YUV420:
    bytes_read = fread( &( m_pppcInputPel[0][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight ) )
    {
      m_iErrorStatus = READING;
      qDebug( ) << " Reading error !!!" << endl;
      return;
    }
    bytes_read = fread( &( m_pppcInputPel[1][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight / 4, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight / 4 ) )
    {
      m_iErrorStatus = READING;
      qDebug( ) << " Reading error !!!" << endl;
      return;
    }
    bytes_read = fread( &( m_pppcInputPel[2][0][0] ), sizeof(Pel), m_uiWidth * m_uiHeight / 4, m_pFile );
    if( bytes_read != ( m_uiWidth * m_uiHeight / 4 ) )
    {
      m_iErrorStatus = READING;
      qDebug( ) << " Reading error !!!" << endl;
      return;
    }
    YUV420toRGB();
    break;
  }

  return;
}

Bool InputStream::writeFrame( const QString& filename )
{
  getFrameQImage().save( filename );
  return true;
}

QImage InputStream::getFrameQImage()
{
  // Create QImage
  QImage img( m_uiWidth, m_uiHeight, QImage::Format_RGB888 );

  UInt64 frame_bytes = m_uiWidth * m_uiHeight;
  Pel pelR, pelG, pelB;
  Pel* bufferRGB = &( m_pppcRGBPel[0][0][0] );

  if( sizeof(Pel) == sizeof(unsigned char) )
  {
    for( Int y = 0; y < m_uiHeight; y++ )
    {
      for( Int x = 0; x < m_uiWidth; x++ )
      {
        img.setPixel( x, y, qRgb( m_pppcRGBPel[0][y][x], m_pppcRGBPel[1][y][x], m_pppcRGBPel[2][y][x] ) );
        //img.setPixel( x, y, qRgb( 0, 0, 255 ) );
      }
    }
  }
  else
  {
    Q_ASSERT( 0 );
  }
  return img;
}

#ifdef USE_OPENCV
cv::Mat InputStream::getFrameCvMat()
{
  cv::Mat cvMat( m_uiHeight, m_uiWidth, CV_8UC3 );

  return cvMat;
}
#endif

Void InputStream::seekInput( Int new_frame_num )
{
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;
  UInt64 nbytes_seek = frame_bytes_input * new_frame_num;
  fseek( m_pFile, nbytes_seek, SEEK_SET );
  m_uiCurrFrameNum = new_frame_num;
}

Bool InputStream::checkErrors( Int error_type )
{
  if( m_iErrorStatus == error_type )
  {
    m_iErrorStatus = NO_ERROR;
    return true;
  }
  return false;
}

}  // NAMESPACE
