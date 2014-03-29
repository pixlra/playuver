/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \brief    Input stream handling
 */

#include "config.h"

#include <cstdio>
#include <QtDebug>

#include "InputStream.h"
#include "LibMemAlloc.h"
#include "LibMemory.h"

#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif

namespace plaYUVer
{

InputStream::InputStream()
{
  m_bInit = false;
  m_iErrorStatus = 0;

  m_pFile = NULL;
  m_uiWidth = 0;
  m_uiHeight = 0;
  m_uiTotalFrameNum = 0;
  m_iCurrFrameNum = -1;
  m_iPixelFormat = -1;
  m_uiFrameRate = 30;
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
  formats = "*.yuv "  // Raw video
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
  formats << "Raw video (*.yuv)"
#ifdef USE_FFMPEG
          << "Audio video interleaved (*.avi)"
          << "Windows media video (*.wmv)"
#endif
          ;
  return formats;
}

QStringList InputStream::supportedWriteFormatsList()
{
  QStringList formats;
  formats << "Windows Bitmap (*.bmp)"
          << "Joint Photographic Experts Group (*.jpg *.jpeg)"
          << "Portable Network Graphics (*.png)";
  return formats;
}

Bool InputStream::guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate )
{
  Bool bRet = false;
  QString fileExtension = QFileInfo( filename ).completeSuffix();
  if( !fileExtension.compare( "yuv" ) )
  {
    bRet = true;
    // Guess resolution - match %dx%d
    QRegularExpressionMatch resolutionMatch = QRegularExpression( "_\\d*x\\d*_" ).match( filename );
    if( resolutionMatch.hasMatch() )
    {
      QString resolutionString = resolutionMatch.captured( 0 );
      if( resolutionString.startsWith( "_" ) && resolutionString.endsWith( "_" ) )
      {
        resolutionString.remove( "_" );
        QStringList resolutionArgs = resolutionString.split( "x" );
        qDebug( ) << "Found resolution = " << resolutionArgs;
        if( resolutionArgs.size() == 2 )
        {
          rWidth = resolutionArgs.at( 0 ).toUInt();
          rHeight = resolutionArgs.at( 1 ).toUInt();
        }
      }
    }
    // Guess pixel format
    QStringList formats_list = PlaYUVerFrame::supportedPixelFormatList();
    for( Int i = 0; i < formats_list.size(); i++ )
    {
      if( filename.contains( formats_list.at( i ), Qt::CaseInsensitive ) )
      {
        rInputFormat = i;
        break;
      }
    }
  }
  return bRet;
}

Bool InputStream::open( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate )
{
  Bool avStatus = false;

  if( m_bInit )
  {
    close();
  }

  m_bInit = false;
  m_cFilename = filename;
  m_uiWidth = width;
  m_uiHeight = height;
  m_uiFrameRate = frame_rate;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

#ifdef USE_FFMPEG
  if( QFileInfo( filename ).completeSuffix().compare( QString( "yuv" ) ) )
  {
    avStatus = m_cLibAvContext.initAvFormat( m_cFilename.toLocal8Bit().data(), m_uiWidth, m_uiHeight, m_iPixelFormat, m_uiFrameRate );
    m_cFormatName = QString::fromUtf8( m_cLibAvContext.getCodecName() );

    m_uiTotalFrameNum = 100;
  }
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 )
  {
    return m_bInit;
  }

  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();

  if( !avStatus )
  {
    m_pFile = fopen( m_cFilename.toLocal8Bit().data(), "rb" );
    if( m_pFile == NULL )
    {
      close();
      return m_bInit;
    }
    fseek( m_pFile, 0, SEEK_END );
    m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
    fseek( m_pFile, 0, SEEK_SET );
    m_cFormatName = QString::fromUtf8( "Raw Video" );
  }
  if( !getMem1D<Pel>( &m_pInputBuffer, m_pcCurrFrame->getBytesPerFrame() ) )
  {
    close();
    return m_bInit;
  }

  m_cStreamInformationString = QString( "[" );
  m_cStreamInformationString.append( m_cFormatName );
  m_cStreamInformationString.append( QString( " / " ) );
  m_cStreamInformationString.append( PlaYUVerFrame::supportedPixelFormatList().at( m_iPixelFormat ) );
  m_cStreamInformationString.append( "] " );
  m_cStreamInformationString.append( QFileInfo( m_cFilename ).fileName() );

  m_iCurrFrameNum = -1;
  readNextFrame();
  setNextFrame();
  if( m_uiTotalFrameNum > 1 )
    readNextFrame();

  m_bInit = true;

  return m_bInit;
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

  if( m_iCurrFrameNum + 1 >= ( Int )m_uiTotalFrameNum )
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
    qDebug( ) << " Reading error !!!"
              << endl;
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

  pyuv_image->CopyFrom( m_pcCurrFrame );
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
  if( new_frame_num < 0 || new_frame_num >= ( Int )m_uiTotalFrameNum )
    return;
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
