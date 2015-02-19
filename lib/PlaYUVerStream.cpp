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
 * \file     PlaYUVerStream.cpp
 * \brief    Input stream handling
 */

#include "config.h"
#include <cstdio>
#include <vector>
#include "LibMemory.h"
#include "PlaYUVerStream.h"
#include <QImage>
#include <QString>
#include <QFileInfo>
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif

namespace plaYUVer
{

enum PlaYUVerStreamInputFormats
{
  INVALID_INPUT = -1,
  YUVINPUT = 0,
#ifdef USE_FFMPEG
  AVIINPUT,
  MP4INPUT,
  WMVINPUT,
  PGMINPUT,
  PNGINPUT,
  BMPINPUT,
  JPEGINPUT,
#endif
  TOTAL_INPUT_FORMATS
};

QStringList PlaYUVerStream::supportedReadFormatsExt()
{
  QStringList formatsExt;
  formatsExt << "yuv"
             // Raw video
#ifdef USE_FFMPEG
             << "avi"
             // Audio video interleaved
             << "mp4"
             // MP4
             << "wmv"
             // Windows media video
             << "pgm"
             << "png"
             << "bmp"
             << "jpg"
#endif
             ;
  //assert( formatsExt.size() == TOTAL_INPUT_FORMATS );
  return formatsExt;
}

QStringList PlaYUVerStream::supportedReadFormatsName()
{
  QStringList formatsName;
  formatsName << "Raw video"
#ifdef USE_FFMPEG
              << "Audio video interleaved"
              << "MPEG4"
              << "Windows media video"
              << "Portable Grayscale Map"
              << "Portable Network Graphics"
              << "Windows Bitmap"
              << "Joint Photographic Experts Group"
#endif
              ;
  //assert( formatsName.size() == TOTAL_INPUT_FORMATS );
  return formatsName;
}

enum PlaYUVerStreamOutputFormats
{
  INVALID_OUTPUT = -1,
  YUVOUTPUT = 0,
  TOTAL_OUTPUT_FORMATS
};

QStringList PlaYUVerStream::supportedWriteFormatsExt()
{
  QStringList formatsExt;
  formatsExt << "yuv"   // raw video
  ;
  assert( formatsExt.size() == TOTAL_OUTPUT_FORMATS );
  return formatsExt;
}

QStringList PlaYUVerStream::supportedWriteFormatsName()
{
  QStringList formatsName;
  formatsName << "Raw video";
  assert( formatsName.size() == TOTAL_OUTPUT_FORMATS );
  return formatsName;
}

QStringList PlaYUVerStream::supportedSaveFormatsExt()
{
  QStringList formatsExt;
  formatsExt << supportedWriteFormatsExt()
             << "bmp"
             << "jpeg"
             << "png"  // portable network graphics
             ;
  formatsExt.removeDuplicates();
  return formatsExt;
}

QStringList PlaYUVerStream::supportedSaveFormatsName()
{
  QStringList formatsName;
  formatsName << supportedWriteFormatsName()
              << "Windows Bitmap"
              << "Joint Photographic Experts Group"
              << "Portable Network Graphics";
  formatsName.removeDuplicates();
  return formatsName;
}

#define REGIST_PLAYUVER_STANDARD_RESOLUTION( name, width, height) \
    stdResElement.shortName = name; \
    stdResElement.uiWidth = width; \
    stdResElement.uiHeight = height; \
    stdResList.push_back( stdResElement );

std::vector<PlaYUVerStdResolution> PlaYUVerStream::stdResolutionSizes()
{
  std::vector<PlaYUVerStdResolution> stdResList;
  PlaYUVerStdResolution stdResElement;
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "CIF", 352, 288 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "VGA", 640, 480 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "WVGA", 832, 480 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "XVGA", 1024, 768 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "HD", 1280, 720 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "SXGA-", 1280, 900 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "SXGA", 1280, 1024 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "WSXGA", 1440, 900 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "FullHD", 1920, 1080 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "WQXGA", 2560, 1600 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "UltraHD", 3840, 2160 );
  REGIST_PLAYUVER_STANDARD_RESOLUTION( "8K", 8192, 4608 );
  return stdResList;
}

PlaYUVerStream::PlaYUVerStream()
{
  m_bInit = false;
  m_bIsInput = true;
  m_bIsOpened = false;
  m_bLoadAll = false;
  m_iErrorStatus = 0;
  m_uiStreamHandler = 0;
  m_pFile = NULL;
  m_uiWidth = 0;
  m_uiHeight = 0;
  m_uiTotalFrameNum = 0;
  m_iCurrFrameNum = -1;
  m_iPixelFormat = -1;
  m_uiFrameRate = 30;
  m_iFileFormat = -1;
  m_pStreamBuffer = NULL;
  m_cFilename = "";
  m_pcCurrFrame = NULL;
  m_pcNextFrame = NULL;
  m_ppcFrameBuffer = NULL;
  m_uiFrameBufferSize = 2;
  m_uiFrameBufferIndex = 0;
#ifdef USE_FFMPEG
  m_cLibAvContext = new LibAvContextHandle;
#endif
}

PlaYUVerStream::~PlaYUVerStream()
{
  close();
}

Bool PlaYUVerStream::open( std::string filename, UInt width, UInt height, Int input_format, UInt frame_rate, Bool bInput )
{
  if( m_bInit )
  {
    close();
  }

  QString qtFilename = QString::fromStdString( filename );

  m_bInit = false;
  m_bIsInput = bInput;
  m_cFilename = filename;
  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = input_format;
  m_uiFrameRate = frame_rate;

  if( m_bIsInput )
  {
    m_iFileFormat = INVALID_HANDLER;

    QStringList formatsExt = PlaYUVerStream::supportedReadFormatsExt();
    QString currExt = QFileInfo( qtFilename ).suffix();
    if( formatsExt.contains( currExt ) )
    {
      m_iFileFormat = formatsExt.indexOf( currExt );
    }
    if( m_iFileFormat == INVALID_HANDLER )
    {
      close();
      throw "Invalid file format";
      return m_bInit;
    }
    m_cFormatName = PlaYUVerStream::supportedReadFormatsExt().at( m_iFileFormat ).toUpper().toStdString();

    if( m_iFileFormat == YUVINPUT )
    {
      m_uiStreamHandler = YUV_IO;
    }

#ifdef USE_FFMPEG
    if( m_iFileFormat != YUVINPUT )
    {
      m_uiStreamHandler = FFMPEG;
    }
#endif
#if 0
    if( !avStatus )
    {
      switch( m_iFileFormat )
      {
        case PGMINPUT:

        break;
      }
    }
#endif
    m_uiFrameBufferSize = 2;
  }
  else
  {
    m_iFileFormat = YUVOUTPUT;
    m_uiStreamHandler = YUV_IO;
    m_uiFrameBufferSize = 1;
  }

#ifdef USE_FFMPEG
  if( m_uiStreamHandler == FFMPEG )
  {
    if( !m_cLibAvContext->initAvFormat( m_cFilename.c_str(), m_uiWidth, m_uiHeight, m_iPixelFormat, m_uiFrameRate, m_uiTotalFrameNum ) )
    {
      throw "Cannot open file using FFmpeg libs";
    }
    m_cCodedName = m_cLibAvContext->getCodecName();
  }
#endif

  if( m_uiWidth <= 0 || m_uiHeight <= 0 || m_iPixelFormat < 0 )
  {
    close();
    throw "[PlaYUVerStream] Incorrect configuration";
    return m_bInit;
  }

  if( m_uiStreamHandler == YUV_IO )
  {
    if( !openFile() )
    {
      close();
      throw "[PlaYUVerStream] Cannot open file";
      return m_bInit;
    }
    m_cFormatName = "YUV";
    m_cCodedName = "Raw Video";
  }

  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
    if( !m_ppcFrameBuffer[i] )
    {
      close();
      throw "[PlaYUVerStream] Cannot allocated frame buffer";
      return m_bInit;
    }
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();

  if( m_uiStreamHandler == YUV_IO && m_bIsInput )
  {
    fseek( m_pFile, 0, SEEK_END );
    UInt64 fileSize = ftell( m_pFile );
#if 0
    if( fileSize % frame_bytes_input )
    {
      Int remaining = fileSize % frame_bytes_input;
      close();
      throw "[PlaYUVerStream] Incorrect configuration: failed ( fileSize % frame_bytes_input = " << remaining << " )";
      return m_bInit;
    }
#endif
    m_uiTotalFrameNum = fileSize / frame_bytes_input;
    fseek( m_pFile, 0, SEEK_SET );
  }

  if( m_bIsInput && m_uiTotalFrameNum <= 0 )
  {
    close();
    throw "[PlaYUVerStream] Incorrect configuration: less than one frame";
    return m_bInit;
  }

  if( !getMem1D<Pel>( &m_pStreamBuffer, m_pcCurrFrame->getBytesPerFrame() ) )
  {
    close();
    throw "[PlaYUVerStream] Cannot allocated memory";
    return m_bInit;
  }

  m_cPelFmtName = PlaYUVerFrame::supportedPixelFormatListNames()[m_iPixelFormat].c_str();

  m_iCurrFrameNum = -1;

  m_bInit = true;

  seekInput( 0 );

  m_bInit = true;
  return m_bInit;
}

Bool PlaYUVerStream::openFile()
{
  m_pFile = NULL;
  m_pFile = fopen( m_cFilename.c_str(), m_bIsInput ? "rb" : "wb" );
  if( m_pFile == NULL )
  {
    return false;
  }
  fseek( m_pFile, m_iCurrFrameNum >= 0 ? m_iCurrFrameNum : 0, SEEK_SET );
  return true;
}

Void PlaYUVerStream::closeFile()
{
  if( m_pFile )
    fclose( m_pFile );
}

Void PlaYUVerStream::close()
{
  if( !m_bInit )
    return;

  if( m_uiStreamHandler == YUV_IO )
    closeFile();

#ifdef USE_FFMPEG
  if( m_uiStreamHandler == FFMPEG )
    m_cLibAvContext->closeAvFormat();
  delete m_cLibAvContext;
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
    rInputFormat = PlaYUVerFrame::YUV420p;
    rFrameRate = 30;
  }
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
  setNextFrame();
  for( UInt i = m_uiFrameBufferIndex; i < m_uiFrameBufferSize; i++ )
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
  if( m_cLibAvContext->getStatus() )
  {
    secs = m_cLibAvContext->getStreamDuration();
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
  if( m_uiStreamHandler == FFMPEG )
  {
    if( !m_cLibAvContext->decodeAvFormat() )
    {
      m_iErrorStatus = READING;
      return;
    }
    m_pcNextFrame->frameFromBuffer( m_cLibAvContext->m_pchFrameBuffer, m_cLibAvContext->m_uiFrameBufferSize );
  }
#endif
  if( m_uiStreamHandler == YUV_IO )
  {
    UInt64 frame_bytes_input = m_pcNextFrame->getBytesPerFrame();
    UInt64 bytes_read = fread( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
    if( bytes_read != frame_bytes_input )
    {
      m_iErrorStatus = READING;
      throw "[PlaYUVerStream] Cannot read file";
      return;
    }
    m_pcNextFrame->frameFromBuffer( m_pStreamBuffer, bytes_read );
  }
  m_pcNextFrame->fillRGBBuffer();
  return;
}

Void PlaYUVerStream::writeFrame()
{
  if( m_uiStreamHandler == YUV_IO )
  {
    UInt64 frame_bytes_input = m_pcCurrFrame->getBytesPerFrame();
    m_pcCurrFrame->frameToBuffer( m_pStreamBuffer );
    UInt64 bytes_read = fwrite( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
    if( bytes_read != frame_bytes_input )
    {
      m_iErrorStatus = WRITING;
    }
  }
  return;
}

Void PlaYUVerStream::writeFrame( PlaYUVerFrame *pcFrame )
{
  if( m_uiStreamHandler == YUV_IO )
  {
    UInt64 frame_bytes_input = pcFrame->getBytesPerFrame();
    pcFrame->frameToBuffer( m_pStreamBuffer );
    UInt64 bytes_read = fwrite( m_pStreamBuffer, sizeof(Pel), frame_bytes_input, m_pFile );
    if( bytes_read != frame_bytes_input )
    {
      m_iErrorStatus = WRITING;
    }
  }
  return;
}

Bool PlaYUVerStream::saveFrame( const std::string& filename )
{
  return saveFrame( filename, m_pcCurrFrame );
}

Bool PlaYUVerStream::saveFrame( const std::string& filename, PlaYUVerFrame *saveFrame )
{
  QString qtString = QString::fromStdString( filename );
  Int iFileFormat = INVALID_OUTPUT;

  QStringList formatsExt = PlaYUVerStream::supportedSaveFormatsExt();
  QString currExt = QFileInfo( qtString ).suffix();
  if( formatsExt.contains( currExt ) )
  {
    iFileFormat = formatsExt.indexOf( currExt );
  }

  if( iFileFormat < TOTAL_OUTPUT_FORMATS )
  {
    PlaYUVerStream auxFrameStream;
    if( !auxFrameStream.open( filename, saveFrame->getWidth(), saveFrame->getHeight(), saveFrame->getPelFormat(), 1, false ) )
    {
      return false;
    }
    auxFrameStream.writeFrame( saveFrame );
    auxFrameStream.close();
    return true;
  }
  else
  {
    saveFrame->fillRGBBuffer();
    QImage qimg = QImage( saveFrame->getRGBBuffer(), saveFrame->getWidth(), saveFrame->getHeight(), QImage::Format_RGB32 );
    return qimg.save( qtString );
  }
  return false;
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

Bool PlaYUVerStream::seekInput( UInt64 new_frame_num )
{
  if( !m_bInit || new_frame_num < 0 || new_frame_num >= m_uiTotalFrameNum || ( Int64 )new_frame_num == m_iCurrFrameNum )
    return false;

  m_iCurrFrameNum = new_frame_num - 1;

  if( m_bLoadAll )
  {
    m_uiFrameBufferIndex = new_frame_num;
    m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
    setNextFrame();
    return true;
  }
#ifdef USE_FFMPEG
  if( m_uiStreamHandler == FFMPEG )
  {
    m_cLibAvContext->seekAvFormat( new_frame_num );
  }
#endif
  if( m_uiStreamHandler == YUV_IO )
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
  return true;
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
