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
 * \file     PlaYUVerStream.cpp
 * \brief    Input stream handling
 */

// Self
#include "PlaYUVerStream.h"
#include "config.h"

// System
#include <cstdio>

// Own
#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerStreamHandlerIf.h"
#include "StreamHandlerPortableMap.h"
#include "StreamHandlerRaw.h"
#ifdef USE_FFMPEG
#include "StreamHandlerLibav.h"
#endif
#ifdef USE_OPENCV
#include "StreamHandlerOpenCV.h"
#endif

class PlaYUVerStreamBufferPrivate
{
private:
  std::vector<PlaYUVerFrame*> m_apcFrameBuffer;
  UInt m_uiIndex;

public:
  PlaYUVerStreamBufferPrivate( UInt size, UInt width, UInt height, Int pelFormat, Int bitsPixel )
  {
    for( UInt i = 0; i < size; i++ )
    {
      PlaYUVerFrame* pFrame = new PlaYUVerFrame( width, height, pelFormat, bitsPixel );
      m_apcFrameBuffer.push_back( pFrame );
    }
    m_uiIndex = 0;
  }

  ~PlaYUVerStreamBufferPrivate()
  {
    while( m_apcFrameBuffer.size() > 0 )
    {
      delete m_apcFrameBuffer.back();
      m_apcFrameBuffer.pop_back();
    }
  }
  Void increase( UInt newSize )
  {
    for( UInt i = m_apcFrameBuffer.size(); i < newSize; i++ )
    {
      PlaYUVerFrame* pFrame = new PlaYUVerFrame( m_apcFrameBuffer.at( 0 ) );
      m_apcFrameBuffer.push_back( pFrame );
    }
  }
  UInt size() { return m_apcFrameBuffer.size(); }
  Void setIndex( UInt i ) { m_uiIndex = i; }
  PlaYUVerFrame* frame( Int i ) { return m_apcFrameBuffer.at( i ); }

  PlaYUVerFrame* current() { return m_apcFrameBuffer.at( m_uiIndex ); }
  PlaYUVerFrame* next() { return m_apcFrameBuffer.at( nextIndex() ); }
  Void setNextFrame() { m_uiIndex = nextIndex(); }

private:
  inline Int nextIndex() { return m_uiIndex + 1 >= m_apcFrameBuffer.size() ? 0 : m_uiIndex + 1; }
  inline Int prevIndex() { return m_uiIndex - 1 < 0 ? m_apcFrameBuffer.size() - 1 : m_uiIndex - 1; }
};

std::vector<PlaYUVerSupportedFormat> PlaYUVerStream::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw YUV Video", "yuv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw Gray Video", "gray" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw RGB Video", "rgb" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable BitMap ", "pbm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable PixMap ", "ppm" );
#ifdef USE_FFMPEG
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerLibav, Read );
#endif
#ifdef USE_OPENCV
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerOpenCV, Read );
#endif
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> PlaYUVerStream::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw Video", "yuv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable BitMap ", "pbm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
#ifdef USE_FFMPEG
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerLibav, Write );
#endif
#ifdef USE_OPENCV
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerOpenCV, Write );
#endif
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerStdResolution> PlaYUVerStream::stdResolutionSizes()
{
#define REGIST_PLAYUVER_STANDARD_RESOLUTION( name, width, height ) \
  stdResElement.shortName = name;                                  \
  stdResElement.uiWidth = width;                                   \
  stdResElement.uiHeight = height;                                 \
  stdResList.push_back( stdResElement );

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

CreateStreamHandlerFn PlaYUVerStream::findStreamHandler( String strFilename, bool bRead )
{
  String currExt = strFilename.substr( strFilename.find_last_of( "." ) + 1 );
  currExt = lowercase( currExt );

  std::vector<PlaYUVerSupportedFormat> supportedFmts;
  if( bRead )
  {
    supportedFmts = PlaYUVerStream::supportedReadFormats();
  }
  else
  {
    supportedFmts = PlaYUVerStream::supportedWriteFormats();
  }
  for( UInt i = 0; i < supportedFmts.size(); i++ )
  {
    if( currExt != "" && supportedFmts[i].formatExt == currExt )
    {
      return supportedFmts[i].formatFct;
    }
    else if( strFilename.find( supportedFmts[i].formatExt ) != String::npos )
    {
      return supportedFmts[i].formatFct;
    }
  }
  return &StreamHandlerRaw::Create;
}

PlaYUVerStream::PlaYUVerStream()
{
  m_bInit = false;
  m_pfctCreateHandler = NULL;
  m_pcHandler = NULL;
  m_bIsInput = true;
  m_bLoadAll = false;
  m_uiTotalFrameNum = 0;
  m_iCurrFrameNum = -1;
  m_cFilename = "";
}

PlaYUVerStream::~PlaYUVerStream()
{
  close();
}

String PlaYUVerStream::getFormatName()
{
  return !m_pcHandler ? "" : m_pcHandler->getFormatName();
}
String PlaYUVerStream::getCodecName()
{
  return !m_pcHandler ? "" : m_pcHandler->getCodecName();
}

Bool PlaYUVerStream::open( String filename,
                           String resolution,
                           String input_format_name,
                           UInt bitsPel,
                           Int endianness,
                           UInt frame_rate,
                           Bool bInput )
{
  UInt width = 0;
  UInt height = 0;
  Int input_format = -1;

  if( resolution.size() > 0 )
  {
    sscanf( resolution.c_str(), "%ux%u", &width, &height );
    if( width <= 0 || height <= 0 )
    {
      return false;
    }
  }
  for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames().size(); i++ )
  {
    if( lowercase( PlaYUVerFrame::supportedPixelFormatListNames()[i] ) ==
        lowercase( input_format_name ) )
    {
      input_format = i;
      break;
    }
  }
  return open( filename, width, height, input_format, bitsPel, endianness, frame_rate, bInput );
}

Bool PlaYUVerStream::open( String filename,
                           UInt width,
                           UInt height,
                           Int input_format,
                           UInt bitsPel,
                           Int endianness,
                           UInt frame_rate,
                           Bool bInput )
{
  if( m_bInit )
  {
    close();
  }
  m_bInit = false;
  m_bIsInput = bInput;

  m_pfctCreateHandler = PlaYUVerStream::findStreamHandler( filename, m_bIsInput );
  if( !m_pfctCreateHandler )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Invalid handler" );
  }

  m_pcHandler = m_pfctCreateHandler();

  if( !m_pcHandler )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot create handler" );
  }

  m_cFilename = filename;

  m_pcHandler->m_cFilename = filename;
  m_pcHandler->m_uiWidth = width;
  m_pcHandler->m_uiHeight = height;
  m_pcHandler->m_iPixelFormat = input_format;
  m_pcHandler->m_uiBitsPerPixel = bitsPel;
  m_pcHandler->m_iEndianness = endianness;
  m_pcHandler->m_dFrameRate = frame_rate;

  if( !m_pcHandler->openHandler( m_cFilename, m_bIsInput ) )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot open file " + m_cFilename );
    return m_bInit;
  }

  if( m_pcHandler->m_uiWidth <= 0 || m_pcHandler->m_uiHeight <= 0 ||
      m_pcHandler->m_iPixelFormat < 0 )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Incorrect configuration" );
    return m_bInit;
  }

  // Keep past, current and future frames
  try
  {
    m_frameBuffer = new PlaYUVerStreamBufferPrivate(
        m_bIsInput ? 3 : 1, m_pcHandler->m_uiWidth, m_pcHandler->m_uiHeight,
        m_pcHandler->m_iPixelFormat, m_pcHandler->m_uiBitsPerPixel );
  }
  catch( PlaYUVerFailure& e )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated frame buffer" );
    return m_bInit;
  }

  m_pcHandler->setBytesPerFrame( m_frameBuffer->current()->getBytesPerFrame() );

  if( m_bIsInput )
  {
    m_uiTotalFrameNum = m_pcHandler->calculateFrameNumber();
  }

  if( m_bIsInput && m_uiTotalFrameNum <= 0 )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Incorrect configuration: less than one frame" );
    return m_bInit;
  }

  if( !m_pcHandler->configureBuffer( m_frameBuffer->current() ) )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated memory" );
    return m_bInit;
  }

  m_iCurrFrameNum = -1;
  m_bInit = true;

  seekInput( 0 );

  m_bInit = true;
  return m_bInit;
}

Bool PlaYUVerStream::reload()
{
  m_pcHandler->closeHandler();
  if( !m_pcHandler->openHandler( m_cFilename, m_bIsInput ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot open file" );
  }
  m_pcHandler->setBytesPerFrame( m_frameBuffer->current()->getBytesPerFrame() );
  m_uiTotalFrameNum = m_pcHandler->calculateFrameNumber();

  if( m_pcHandler->m_uiWidth <= 0 || m_pcHandler->m_uiHeight <= 0 ||
      m_pcHandler->m_iPixelFormat < 0 || m_pcHandler->m_uiBitsPerPixel == 0 ||
      m_uiTotalFrameNum < 1 )
  {
    return false;
  }
  if( UInt( m_iCurrFrameNum ) >= m_uiTotalFrameNum )
  {
    m_iCurrFrameNum = 0;
  }
  Int currFrameNum = m_iCurrFrameNum;
  m_iCurrFrameNum = -1;
  seekInput( currFrameNum );
  return true;
}

Void PlaYUVerStream::close()
{
  if( !m_bInit )
    return;

  m_pcHandler->closeHandler();
  m_pcHandler->Delete();

  delete m_frameBuffer;

  m_bLoadAll = false;
  m_bInit = false;
}

String PlaYUVerStream::getFileName()
{
  return m_cFilename;
}

UInt PlaYUVerStream::getFrameNum()
{
  return m_uiTotalFrameNum;
}
UInt PlaYUVerStream::getWidth() const
{
  return m_pcHandler->m_uiWidth;
}
UInt PlaYUVerStream::getHeight() const
{
  return m_pcHandler->m_uiHeight;
}
Int PlaYUVerStream::getEndianess() const
{
  return m_pcHandler->m_iEndianness;
}
Double PlaYUVerStream::getFrameRate()
{
  return m_pcHandler->m_dFrameRate;
}

Int PlaYUVerStream::getCurrFrameNum()
{
  return m_iCurrFrameNum;
}

Void PlaYUVerStream::getFormat( UInt& rWidth,
                                UInt& rHeight,
                                Int& rInputFormat,
                                UInt& rBitsPerPel,
                                Int& rEndianness,
                                UInt& rFrameRate )
{
  if( m_bInit )
  {
    rWidth = m_pcHandler->m_uiWidth;
    rHeight = m_pcHandler->m_uiHeight;
    rInputFormat = m_pcHandler->m_iPixelFormat;
    rBitsPerPel = m_pcHandler->m_uiBitsPerPixel;
    rEndianness = m_pcHandler->m_iEndianness;
    rFrameRate = m_pcHandler->m_dFrameRate;
  }
  else
  {
    rWidth = 0;
    rHeight = 0;
    rInputFormat = PlaYUVerFrame::YUV420p;
    rBitsPerPel = 8;
    rEndianness = 0;
    rFrameRate = 30;
  }
}

Void PlaYUVerStream::loadAll()
{
  if( m_bLoadAll || !m_bIsInput )
    return;

  try
  {
    m_frameBuffer->increase( m_uiTotalFrameNum );
  }
  catch( PlaYUVerFailure& e )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated frame buffer" );
  }

  seekInput( 0 );
  for( UInt i = 2; i < m_frameBuffer->size(); i++ )
  {
    readFrame( m_frameBuffer->frame( i ) );
  }
  m_bLoadAll = true;
  m_iCurrFrameNum = 0;
}

Void PlaYUVerStream::getDuration( Int* duration_array )
{
  //   Int hours, mins, secs = 0;
  // #ifdef USE_FFMPEG
  //   if( m_iStreamHandler == FFMPEG )
  //   {
  //     secs = m_cLibAvContext->getStreamDuration();
  //   }
  //   else
  // #endif
  //   if( m_iStreamHandler == YUV_IO )
  //   {
  //     secs = m_uiTotalFrameNum / m_dFrameRate;
  //   }
  //   mins = secs / 60;
  //   secs %= 60;
  //   hours = mins / 60;
  //   mins %= 60;
  //   *duration_array++ = hours;
  //   *duration_array++ = mins;
  //   *duration_array++ = secs;
}

Bool PlaYUVerStream::readFrame( PlaYUVerFrame* frame )
{
  if( !m_bInit || !m_bIsInput || m_pcHandler->m_uiCurrFrameFileIdx >= m_uiTotalFrameNum )
    return false;

  if( m_bLoadAll )
    return true;

  if( !m_pcHandler->read( frame ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot read file" );
    return false;
  }
  return true;
}

Void PlaYUVerStream::writeFrame()
{
  writeFrame( m_frameBuffer->current() );
}

Void PlaYUVerStream::writeFrame( PlaYUVerFrame* pcFrame )
{
  if( !m_pcHandler->write( pcFrame ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot write into the file" );
  }
  return;
}

Bool PlaYUVerStream::saveFrame( const String& filename )
{
  return saveFrame( filename, m_frameBuffer->current() );
}

Bool PlaYUVerStream::saveFrame( const String& filename, PlaYUVerFrame* saveFrame )
{
  PlaYUVerStream auxSaveStream;
  if( !auxSaveStream.open( filename, saveFrame->getWidth(), saveFrame->getHeight(),
                           saveFrame->getPelFormat(), saveFrame->getBitsPel(),
                           PLAYUVER_LITTLE_ENDIAN, 1, false ) )
  {
    return false;
  }
  auxSaveStream.writeFrame( saveFrame );
  auxSaveStream.close();
  return true;
}

Bool PlaYUVerStream::setNextFrame()
{
  Bool bEndOfSeq = false;

  if( m_iCurrFrameNum + 1 < Int( m_uiTotalFrameNum ) )
  {
    m_frameBuffer->setNextFrame();
    m_iCurrFrameNum++;
  }
  else
  {
    bEndOfSeq = true;
  }
  return bEndOfSeq;
}

Void PlaYUVerStream::readNextFrame()
{
  readFrame( m_frameBuffer->next() );
}

Void PlaYUVerStream::readNextFrameFillRGBBuffer()
{
  readNextFrame();
  m_frameBuffer->next()->fillRGBBuffer();
  return;
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame( PlaYUVerFrame* pyuv_image )
{
  if( pyuv_image == NULL )
    pyuv_image = new PlaYUVerFrame( m_frameBuffer->current() );
  else
    pyuv_image->copyFrom( m_frameBuffer->current() );
  return pyuv_image;
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame()
{
  return m_frameBuffer->current();
}

Bool PlaYUVerStream::seekInputRelative( Bool bIsFoward )
{
  if( !m_bInit || !m_bIsInput )
    return false;

  Bool bRet = false;
  if( bIsFoward )
  {
    bRet = !setNextFrame();
    readFrame( m_frameBuffer->next() );
  }
  else
  {
    UInt64 newFrameNum = m_iCurrFrameNum - 1;
    bRet = seekInput( newFrameNum );
  }
  return bRet;
}

Bool PlaYUVerStream::seekInput( UInt64 new_frame_num )
{
  if( !m_bInit || new_frame_num >= m_uiTotalFrameNum || (Int64)new_frame_num == m_iCurrFrameNum )
    return false;

  m_iCurrFrameNum = new_frame_num;

  if( m_bLoadAll )
  {
    m_frameBuffer->setIndex( m_iCurrFrameNum );
    return true;
  }

  if( !m_pcHandler->seek( m_iCurrFrameNum ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot write into the file" );
  }

  m_frameBuffer->setIndex( 0 );
  readFrame( m_frameBuffer->current() );
  if( m_uiTotalFrameNum > 1 )
    readFrame( m_frameBuffer->next() );

  return true;
}
