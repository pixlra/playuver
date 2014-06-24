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
 * \file     PlaYUVerStream.cpp
 * \brief    Input stream handling
 */

#include <cstdio>
#include "PlaYUVerStream.h"
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif
#include "LibMemory.h"

namespace plaYUVer
{

PlaYUVerStream::PlaYUVerStream()
{
  m_bInit = false;
  m_bIsInput = true;
  m_bLoadAll = false;
  m_iErrorStatus = 0;

  m_pFile = NULL;
  m_uiWidth = 0;
  m_uiHeight = 0;
  m_uiTotalFrameNum = 0;
  m_iCurrFrameNum = -1;
  m_iPixelFormat = -1;
  m_uiFrameRate = 30;
  m_iFileFormat = INVALID;
  m_pStreamBuffer = NULL;
  m_cFilename = QString( "" );
  m_cStreamInformationString = QString( "" );
  m_pcCurrFrame = NULL;
  m_pcNextFrame = NULL;
  m_ppcFrameBuffer = NULL;
  m_uiFrameBufferSize = 2;
  m_uiFrameBufferIndex = 0;
  m_uiAveragePlayInterval = 0;
}

PlaYUVerStream::~PlaYUVerStream()
{
  close();
}

QString PlaYUVerStream::supportedReadFormats()
{
  QString formats;
  formats = "*.yuv "  // Raw video
#ifdef USE_FFMPEG
      "*.avi "   // Audio video interleaved
      "*.mp4 "// MP4
      "*.wmv "// Windows media video
#endif
;  return formats;
}

QStringList PlaYUVerStream::supportedReadFormatsList()
{
  QStringList formats;
  formats << "Raw video (*.yuv)"
#ifdef USE_FFMPEG
          << "Audio video interleaved (*.avi)"
          << "MPEG4 (*.mp4)"
          << "Windows media video (*.wmv)"
#endif
          ;
  return formats;
}

QString PlaYUVerStream::supportedWriteFormats()
{
  QString formats;
  formats = "*.yuv "   // raw video
      ;
  return formats;
}

QStringList PlaYUVerStream::supportedWriteFormatsList()
{
  QStringList formats;
  formats << "Raw video (*.yuv)";
  return formats;
}

QString PlaYUVerStream::supportedSaveFormats()
{
  QString formats;
  formats = "*.jpg "   // Joint Photographic Experts Group
          "*.jpeg "// Joint Photographic Experts Group
          "*.png "// Portable Network Graphics
  ;
  return formats;
}

QStringList PlaYUVerStream::supportedSaveFormatsList()
{
  QStringList formats;
  formats << "Windows Bitmap (*.bmp)"
          << "Joint Photographic Experts Group (*.jpg *.jpeg)"
          << "Portable Network Graphics (*.png)";
  return formats;
}

Bool PlaYUVerStream::open( QString filename, UInt width, UInt height, Int input_format, UInt frame_rate, Bool bInput )
{
  Bool avStatus = false;

  if( m_bInit )
  {
    close();
  }

  m_bInit = false;
  m_bIsInput = bInput;
  m_cFilename = filename;
  m_uiWidth = width;
  m_uiHeight = height;
  m_uiFrameRate = frame_rate;

  m_iFileFormat = YUVFormat;
  m_iPixelFormat = input_format;

#ifdef USE_FFMPEG
  if( m_bIsInput && QFileInfo( filename ).suffix().compare( QString( "yuv" ) ) )
  {
    avStatus = m_cLibAvContext.initAvFormat( m_cFilename.toLocal8Bit().data(), m_uiWidth, m_uiHeight, m_iPixelFormat, m_uiFrameRate, m_uiTotalFrameNum );
    m_cFormatName = QFileInfo( filename ).completeSuffix().toUpper();
    m_cCodedName = QString::fromUtf8( m_cLibAvContext.getCodecName() ).toUpper();
  }
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 || m_iPixelFormat < 0 )
  {
    close();
    return m_bInit;
  }

  if( m_bIsInput )
  {
    m_uiFrameBufferSize = 2;
  }
  else
  {
    m_uiFrameBufferSize = 1;
  }
  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
    if( !m_ppcFrameBuffer[i] )
    {
      close();
      return m_bInit;
    }
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();

  if( !avStatus )
  {
    m_pFile = NULL;
    Char *filename = new char[m_cFilename.length() + 1];
    memcpy( filename, m_cFilename.toUtf8().constData(), m_cFilename.length() + 1 * sizeof(char) );
    m_pFile = fopen( filename, m_bIsInput ? "rb" : "wb" );
    delete[] filename;
    if( m_pFile == NULL )
    {
      close();
      return m_bInit;
    }
    if( m_bIsInput )
    {
      fseek( m_pFile, 0, SEEK_END );
      m_uiTotalFrameNum = ftell( m_pFile ) / ( frame_bytes_input );
      fseek( m_pFile, 0, SEEK_SET );
    }
    m_cFormatName = QString::fromUtf8( "YUV" );
    m_cCodedName = QString::fromUtf8( "Raw Video" );
  }

  if( !getMem1D<Pel>( &m_pStreamBuffer, m_pcCurrFrame->getBytesPerFrame() ) )
  {
    close();
    return m_bInit;
  }

  m_cStreamInformationString = QString( "[" );
  m_cStreamInformationString.append( m_cFormatName );
  m_cStreamInformationString.append( QString( " / " ) );
  m_cStreamInformationString.append( m_cCodedName );
  m_cStreamInformationString.append( QString( " / " ) );
  m_cStreamInformationString.append( PlaYUVerFrame::supportedPixelFormatList().at( m_iPixelFormat ) );
  m_cStreamInformationString.append( "] " );
  m_cStreamInformationString.append( QFileInfo( m_cFilename ).fileName() );

  m_iCurrFrameNum = -1;

  m_bInit = true;

  seekInput( 0 );
  //loadAll();

  m_cTimer.start();
  m_uiAveragePlayInterval = 0;

  m_bInit = true;
  return m_bInit;
}

Void PlaYUVerStream::close()
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
      if( m_ppcFrameBuffer[i] )
        delete m_ppcFrameBuffer[i];
      m_ppcFrameBuffer[i] = NULL;
    }
    freeMem1D<PlaYUVerFrame*>( m_ppcFrameBuffer );
    m_ppcFrameBuffer = NULL;
  }

  if( m_pStreamBuffer )
    freeMem1D<Pel>( m_pStreamBuffer );

  qDebug( ) << "Frame read time: "
            << QString::number( 1000 / ( m_uiAveragePlayInterval + 1 ) )
            << " fps";

  m_bLoadAll = false;
  m_bInit = false;
}

Void PlaYUVerStream::getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate )
{
  if( m_bInit )
  {
    rWidth = m_uiWidth;
    rHeight = m_uiHeight;
    rInputFormat = m_iPixelFormat;
    rFrameRate = m_uiFrameRate;
  }
  else
  {
    rWidth = 0;
    rHeight = 0;
    rInputFormat = -1;
    rFrameRate = 30;
  }
}

Bool PlaYUVerStream::guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate )
{
  Bool bRet = false;
  QString fileExtension = QFileInfo( filename ).suffix();
  if( !fileExtension.compare( "yuv" ) )
  {
    bRet = true;
    // Guess resolution - match %dx%d
#if( QT_VERSION_PLAYUVER == 5 )
    QRegularExpressionMatch resolutionMatch = QRegularExpression( "_\\d*x\\d*_" ).match( filename );
    if( resolutionMatch.hasMatch() )
    {
      QString resolutionString = resolutionMatch.captured( 0 );
      if( resolutionString.startsWith( "_" ) && resolutionString.endsWith( "_" ) )
      {
        resolutionString.remove( "_" );
        QStringList resolutionArgs = resolutionString.split( "x" );
        qDebug( ) << "Found resolution = "
        << resolutionArgs;
        if( resolutionArgs.size() == 2 )
        {
          rWidth = resolutionArgs.at( 0 ).toUInt();
          rHeight = resolutionArgs.at( 1 ).toUInt();
        }
      }
    }
#endif
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
    if( rWidth > 0 && rHeight > 0 && rInputFormat >= 0 )
      bRet = false;
  }
  return bRet;
}

Void PlaYUVerStream::loadAll()
{
  if( m_bLoadAll || !m_bIsInput )
    return;

  if( m_ppcFrameBuffer )
  {
    for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
    {
      if( m_ppcFrameBuffer[i] )
        delete m_ppcFrameBuffer[i];
      m_ppcFrameBuffer[i] = NULL;
    }
    freeMem1D<PlaYUVerFrame*>( m_ppcFrameBuffer );
    m_ppcFrameBuffer = NULL;
  }
  m_uiFrameBufferSize = m_uiTotalFrameNum;
  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
    if( !m_ppcFrameBuffer[i] )
    {
      close();
    }
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
  m_iCurrFrameNum = -1;
  seekInput( 0 );
  for( UInt i = 0; i < m_uiFrameBufferSize - 1; i++ )
  {
    readFrame();
    setNextFrame();
  }
  m_bLoadAll = true;
  seekInput( 0 );
}

Void PlaYUVerStream::getDuration( Int* duration_array )
{
  Int hours, mins, secs;
#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    secs = m_cLibAvContext.getStreamDuration();
  }
  else
#endif
  {
    secs = m_uiTotalFrameNum / m_uiFrameRate;
  }
  mins = secs / 60;
  secs %= 60;
  hours = mins / 60;
  mins %= 60;
  *duration_array++ = hours;
  *duration_array++ = mins;
  *duration_array++ = secs;
}

Void PlaYUVerStream::readFrame()
{
  if( !m_bInit || !m_bIsInput )
    return;

  if( m_iCurrFrameNum + 1 >= ( Int64 )m_uiTotalFrameNum )
  {
    m_iErrorStatus = LAST_FRAME;
    m_pcNextFrame = NULL;
    return;
  }

  if( m_bLoadAll )
  {
    return;
  }

#ifdef USE_FFMPEG
  if( m_cLibAvContext.getStatus() )
  {
    m_cLibAvContext.decodeAvFormat();
    m_pcNextFrame->FrameFromBuffer( m_cLibAvContext.video_dst_data[0], m_iPixelFormat );
  }
  else
#endif
  {
    UInt64 frame_bytes_input = m_pcNextFrame->getBytesPerFrame();
    UInt64 bytes_read = fread( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
    if( bytes_read != frame_bytes_input )
    {
      m_iErrorStatus = READING;
      qDebug( ) << " Reading error !!!"
                << endl;
      return;
    }
    m_pcNextFrame->FrameFromBuffer( m_pStreamBuffer, m_iPixelFormat );
  }
  //Int time = m_cTimer.elapsed();
  //m_uiAveragePlayInterval = ( m_uiAveragePlayInterval + time) / 2;
  m_pcNextFrame->FrametoRGB8();
  return;
}

Void PlaYUVerStream::writeFrame()
{
  UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();
  m_pcCurrFrame->FrameToBuffer( m_pStreamBuffer );
  UInt64 bytes_read = fwrite( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
  if( bytes_read != frame_bytes_input )
  {
    m_iErrorStatus = WRITING;
  }
  return;
}

Void PlaYUVerStream::writeFrame( PlaYUVerFrame *pcFrame )
{
  UInt64 frame_bytes_input = pcFrame->getBytesPerFrame();
  pcFrame->FrameToBuffer( m_pStreamBuffer );
  UInt64 bytes_read = fwrite( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
  if( bytes_read != frame_bytes_input )
  {
    m_iErrorStatus = WRITING;
  }
  return;
}

Void PlaYUVerStream::saveFrame( const QString& filename )
{
  return;
}

Void PlaYUVerStream::setNextFrame()
{
  if( m_pcNextFrame )
  {
    m_pcCurrFrame = m_pcNextFrame;
    m_iCurrFrameNum++;
    m_uiFrameBufferIndex++;
    if( m_uiFrameBufferIndex == m_uiFrameBufferSize )
    {
      m_uiFrameBufferIndex = 0;
    }
    m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
  }
  else
  {
    m_iErrorStatus = END_OF_SEQ;
    seekInput( 0 );
  }
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame( PlaYUVerFrame *pyuv_image )
{
  if( pyuv_image == NULL )
    pyuv_image = new PlaYUVerFrame( m_pcCurrFrame );
  else
    pyuv_image->copyFrom( m_pcCurrFrame );
  return pyuv_image;
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame()
{
  return m_pcCurrFrame;
}

Void PlaYUVerStream::seekInput( UInt64 new_frame_num )
{
  if( !m_pFile || new_frame_num < 0 || new_frame_num >= m_uiTotalFrameNum || ( Int64 )new_frame_num == m_iCurrFrameNum )
    return;

  m_iCurrFrameNum = new_frame_num - 1;

  if( m_bLoadAll )
  {
    m_uiFrameBufferIndex = new_frame_num;
    m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
    setNextFrame();
    return;
  }
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
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  readFrame();
  setNextFrame();
  if( m_uiTotalFrameNum > 1 )
    readFrame();
}

Bool PlaYUVerStream::checkErrors( Int error_type )
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
