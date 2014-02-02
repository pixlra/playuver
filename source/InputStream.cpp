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

  m_iFileFormat = INVALID;
}

InputStream::~InputStream()
{
  if( m_pFile )
    fclose( m_pFile );

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

Void InputStream::init( QString filename, UInt width, UInt height, Int input_format )
{
  m_uiWidth = width;
  m_uiHeight = height;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

#ifdef USE_FFMPEG
  Bool avStatus = m_cLibAvContext.initAvFormat( filename, width, height, input_format );
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 )
  {
    //Error
    return;
  }

  m_cCurrFrame = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );

  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight * 1.5;

  if( m_iFileFormat == YUVFormat )
  {
    m_cFilename = filename;
    m_pFile = fopen( m_cFilename.toLocal8Bit().data(), "rb" );
    if( m_pFile == NULL )
    {
      // Error
      return;
    }
    fseek( m_pFile, 0, SEEK_END );
    fseek( m_pFile, 0, SEEK_SET );
    m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
  }

  if( !get_mem1Dpel( &m_pInputBuffer, m_cCurrFrame->getBytesPerFrame() ) )
  {
    //Error
    return;
  }

  m_iStatus = 1;

  return;
}

Void InputStream::readFrame()
{
  UInt64 bytes_read = 0;

  if( m_iStatus == 0 )
  {
    return;
  }
  UInt64 frame_bytes_input = m_cCurrFrame->getBytesPerFrame();

#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    m_cLibAvContext.decodeAvFormat();
    m_cCurrFrame->FrameFromBuffer( m_cLibAvContext.video_dst_data[0], m_iPixelFormat );
    return;
  }
#endif

  bytes_read = fread( m_pInputBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
  if( bytes_read != frame_bytes_input )
  {
    m_iErrorStatus = READING;
    qDebug() << " Reading error !!!" << endl;
    return;
  }
  m_cCurrFrame->FrameFromBuffer( m_pInputBuffer, m_iPixelFormat );

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
  Pel*** bufferRGB = m_cCurrFrame->getPelBufferRGB();

  if( sizeof(Pel) == sizeof(unsigned char) )
  {
    for( Int y = 0; y < m_uiHeight; y++ )
    {
      for( Int x = 0; x < m_uiWidth; x++ )
      {
        img.setPixel( x, y, qRgb( bufferRGB[0][y][x], bufferRGB[1][y][x], bufferRGB[2][y][x] ) );
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
