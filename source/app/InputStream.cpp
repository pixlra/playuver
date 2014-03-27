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
#include "LibMemory.h"

namespace plaYUVer
{

InputStream::InputStream()
{
  m_iStatus = 0;

  m_pFile = NULL;
  m_uiWidth = 0;
  m_uiHeight = 0;
  m_uiTotalFrameNum = 0;
  m_iCurrFrameNum = -1;
  m_iErrorStatus = 0;
  m_iPixelFormat = -1;

  m_uiFrameRate = 25;

  m_iFileFormat = INVALID;

  m_pInputBuffer = NULL;

  m_cFilename = QString( "" );
  m_cStreamInformationString = QString( "" );

  m_pcCurrFrame = NULL;
  m_pcNextFrame = NULL;
  m_ppcFrameBuffer = NULL;
  m_uiFrameBufferSize = 2;
}

InputStream::~InputStream()
{
  close();
}

QString InputStream::supportedReadFormats()
{
  QString formats;
  formats = "*.yuv "   // Raw video
#ifdef USE_FFMPEG
          "*.avi "   // Audio video interleaved
          "*.wmv "// Windows media video
#endif
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
#ifdef USE_FFMPEG
      << "Audio video interleaved (*.avi)"  // Audio video interleaved
      << "Windows media video (*.wmv)"  // Windows media video
#endif
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

Bool InputStream::needFormatDialog( QString filename )
{
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

Void InputStream::init( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate )
{
  Bool avStatus = false;

  if( m_iStatus == 1 )
  {
    close();
  }

  m_cFilename = filename;
  m_uiWidth = width;
  m_uiHeight = height;
  m_uiFrameRate = frame_rate;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

  m_cStreamInformationString = QString( "[" );

#ifdef USE_FFMPEG
  if( QFileInfo( filename ).completeSuffix().compare( QString( "yuv" ) ) )
  {
    avStatus = m_cLibAvContext.initAvFormat( m_cFilename.toLocal8Bit().data(), m_uiWidth, m_uiHeight, m_iPixelFormat, m_uiFrameRate );
    m_cStreamInformationString.append( QString::fromUtf8( m_cLibAvContext.getStreamInformation() ) );
    m_uiTotalFrameNum = 100;
  }
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 )
  {
    //Error
    return;
  }

  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_pcNextFrame =  m_ppcFrameBuffer[m_uiFrameBufferIndex];

  UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();

  if( !avStatus )
  {
    m_pFile = fopen( m_cFilename.toLocal8Bit().data(), "rb" );
    if( m_pFile == NULL )
    {
      // Error
      return;
    }
    fseek( m_pFile, 0, SEEK_END );
    m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
    fseek( m_pFile, 0, SEEK_SET );

    m_cStreamInformationString.append( QString::fromUtf8("rawvideo ") );
  }

  if( !getMem1D<Pel>( &m_pInputBuffer, m_pcCurrFrame->getBytesPerFrame() ) )
  {
    //Error
    return;
  }

  m_cStreamInformationString.append( QString("/ "));
  m_cStreamInformationString.append( PlaYUVerFrame::supportedPixelFormatList().at( m_iPixelFormat ) );
  m_cStreamInformationString.append( "] " );
  m_cStreamInformationString.append( QFileInfo( m_cFilename ).fileName() );

  m_iCurrFrameNum = -1;
  m_iStatus = 1;

  readNextFrame();
  setNextFrame();
  readNextFrame();
  return;
}

Void InputStream::close()
{
  if( m_pFile )
    fclose( m_pFile );

#ifdef USE_FFMPEG
  m_cLibAvContext.closeAvFormat();
#endif

  if( m_ppcFrameBuffer )
  {
    for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
    {
      delete m_ppcFrameBuffer[i];
    }
    freeMem1D<PlaYUVerFrame*>( m_ppcFrameBuffer );
  }

  if( m_pInputBuffer )
    freeMem1D<Pel>( m_pInputBuffer );

  m_iStatus = 0;
}

Void InputStream::readNextFrame()
{
  UInt64 bytes_read = 0;

  if( m_iStatus == 0 )
  {
    return;
  }

  if( m_iCurrFrameNum + 1 >= (Int)m_uiTotalFrameNum )
  {
    m_iErrorStatus = END_OF_SEQ;
    m_iCurrFrameNum = 0;
    seekInput( m_iCurrFrameNum );
  }

#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    m_cLibAvContext.decodeAvFormat();
    m_pcNextFrame->FrameFromBuffer( m_cLibAvContext.video_dst_data[0], m_iPixelFormat );
    return;
  }
#endif

  UInt64 frame_bytes_input = m_ppcFrameBuffer[0]->getBytesPerFrame();
  bytes_read = fread( m_pInputBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
  if( bytes_read != frame_bytes_input )
  {
    m_iErrorStatus = READING;
    qDebug( ) << " Reading error !!!" << endl;
    return;
  }
  m_pcNextFrame->FrameFromBuffer( m_pInputBuffer, m_iPixelFormat );
  return;
}

Bool InputStream::writeFrame( const QString& filename )
{
  //m_pcCurrFrame->getQimage().save( filename );
  return true;
}

Void InputStream::setNextFrame()
{
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
  m_pcNextFrame = m_ppcFrameBuffer[!m_uiFrameBufferIndex];
  m_uiFrameBufferIndex = !m_uiFrameBufferIndex;
  m_iCurrFrameNum++;
}

PlaYUVerFrame* InputStream::getCurrFrame( PlaYUVerFrame *pyuv_image )
{
  if( pyuv_image == NULL )
    pyuv_image = new PlaYUVerFrame( m_pcCurrFrame->getWidth(), m_pcCurrFrame->getHeight(), m_pcCurrFrame->getPelFormat() );

  pyuv_image->CopyFrom(m_pcCurrFrame);
  return pyuv_image;
}

PlaYUVerFrame* InputStream::getCurrFrame()
{
  return m_pcCurrFrame;
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
#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    m_cLibAvContext.seekAvFormat( new_frame_num );
  }
  else
#endif
  {
    UInt64 frame_bytes_input = m_ppcFrameBuffer[0]->getBytesPerFrame();
    UInt64 nbytes_seek = frame_bytes_input * new_frame_num;
    fseek( m_pFile, nbytes_seek, SEEK_SET );
    m_iCurrFrameNum = new_frame_num - 1;
  }
  readNextFrame();
  setNextFrame();
  readNextFrame();
}

Bool InputStream::checkErrors( Int error_type )
{
  if( m_iErrorStatus == error_type )
  {
    m_iErrorStatus = NO_STREAM_ERROR;
    return true;
  }
  if( m_iErrorStatus == error_type )
  {
    m_iErrorStatus = NO_STREAM_ERROR;
    return true;
  }
  return false;
}

}  // NAMESPACE
