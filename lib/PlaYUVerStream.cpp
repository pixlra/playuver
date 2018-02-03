/*    This file is a part of PlaYUVer project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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

#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerStreamHandlerIf.h"
#include "StreamHandlerPortableMap.h"
#include "StreamHandlerRaw.h"
#include "config.h"
#ifdef USE_FFMPEG
#include "StreamHandlerLibav.h"
#endif
#ifdef USE_OPENCV
#include "StreamHandlerOpenCV.h"
#endif

#include <cstdio>

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

class PlaYUVerStreamPrivate
{
public:
  Bool isInit;
  Bool isInput;

  PlaYUVerStreamHandlerIf* handler;
  CreateStreamHandlerFn pfctCreateHandler;

  PlaYUVerStreamBufferPrivate* frameBuffer;

  String cFilename;
  Int64 iCurrFrameNum;
  Bool bLoadAll;

  PlaYUVerStreamPrivate()
  {
    isInit = false;
    pfctCreateHandler = NULL;
    handler = NULL;
    isInput = true;
    bLoadAll = false;
    iCurrFrameNum = -1;
    cFilename = "";
  }
};

std::vector<PlaYUVerSupportedFormat> PlaYUVerStream::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerRaw, Read );
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerPortableMap, Read );
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
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerRaw, Write );
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerPortableMap, Write );
#ifdef USE_FFMPEG
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerLibav, Write );
#endif
#ifdef USE_OPENCV
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerOpenCV, Write );
#endif
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<String> PlaYUVerSupportedFormat::getExts()
{
  std::vector<String> arrayExt;
  String::size_type prev_pos = 0, pos = 0;
  while( ( pos = formatExt.find( ',', pos ) ) != String::npos )
  {
    String substring( formatExt.substr( prev_pos, pos - prev_pos ) );
    arrayExt.push_back( substring );
    prev_pos = ++pos;
  }
  arrayExt.push_back( formatExt.substr( prev_pos, pos - prev_pos ) );  // Last word
  return arrayExt;
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
    std::vector<String> arrayExt = supportedFmts[i].getExts();
    for( std::vector<String>::iterator e = arrayExt.begin(); e != arrayExt.end(); ++e )
    {
      if( currExt != "" && currExt == *e )
      {
        return supportedFmts[i].formatFct;
      }
      else if( strFilename.find( *e ) != String::npos )
      {
        return supportedFmts[i].formatFct;
      }
    }
  }
  return &StreamHandlerRaw::Create;
}

PlaYUVerStream::PlaYUVerStream()
    : d( new PlaYUVerStreamPrivate )
{
}

PlaYUVerStream::~PlaYUVerStream()
{
  close();
}

String PlaYUVerStream::getFormatName()
{
  return !d->handler ? "" : d->handler->getFormatName();
}
String PlaYUVerStream::getCodecName()
{
  return !d->handler ? "" : d->handler->getCodecName();
}

Bool PlaYUVerStream::open( String filename, String resolution, String input_format_name, UInt bitsPel, Int endianness,
                           UInt frame_rate, Bool bInput )
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
    if( lowercase( PlaYUVerFrame::supportedPixelFormatListNames()[i] ) == lowercase( input_format_name ) )
    {
      input_format = i;
      break;
    }
  }
  return open( filename, width, height, input_format, bitsPel, endianness, frame_rate, bInput );
}

Bool PlaYUVerStream::open( String filename, UInt width, UInt height, Int input_format, UInt bitsPel, Int endianness,
                           UInt frame_rate, Bool bInput )
{
  if( d->isInit )
  {
    close();
  }
  d->isInit = false;
  d->isInput = bInput;

  d->pfctCreateHandler = PlaYUVerStream::findStreamHandler( filename, d->isInput );
  if( !d->pfctCreateHandler )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Invalid handler" );
  }

  d->handler = d->pfctCreateHandler();

  if( !d->handler )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot create handler" );
  }

  d->cFilename = filename;

  d->handler->m_cFilename = filename;
  d->handler->m_uiWidth = width;
  d->handler->m_uiHeight = height;
  d->handler->m_iPixelFormat = input_format;
  d->handler->m_uiBitsPerPixel = bitsPel;
  d->handler->m_iEndianness = endianness;
  d->handler->m_dFrameRate = frame_rate;

  if( !d->handler->openHandler( d->cFilename, d->isInput ) )
  {
    close();
    return d->isInit;
  }

  if( d->handler->m_uiWidth <= 0 || d->handler->m_uiHeight <= 0 || d->handler->m_iPixelFormat < 0 )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Incorrect configuration: width, height or pixel format" );
    return d->isInit;
  }

  // Keep past, current and future frames
  try
  {
    d->frameBuffer = new PlaYUVerStreamBufferPrivate( d->isInput ? 3 : 1, d->handler->m_uiWidth, d->handler->m_uiHeight,
                                                      d->handler->m_iPixelFormat, d->handler->m_uiBitsPerPixel );
  }
  catch( PlaYUVerFailure& e )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated frame buffer" );
    return d->isInit;
  }

  d->handler->m_uiNBytesPerFrame = d->frameBuffer->current()->getBytesPerFrame();

  // Some handlers need to know how long is a frame to get frame number
  d->handler->calculateFrameNumber();

  if( d->isInput && d->handler->m_uiTotalNumberFrames == 0 )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Incorrect configuration: less than one frame" );
    return d->isInit;
  }

  if( !d->handler->configureBuffer( d->frameBuffer->current() ) )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated buffers" );
    return d->isInit;
  }

  d->iCurrFrameNum = -1;
  d->isInit = true;

  seekInput( 0 );

  d->isInit = true;
  return d->isInit;
}

Bool PlaYUVerStream::reload()
{
  d->handler->closeHandler();
  if( !d->handler->openHandler( d->cFilename, d->isInput ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot open stream " + d->cFilename + " with the " +
                                                 String( d->handler->m_pchHandlerName ) + " handler" );
  }
  d->handler->m_uiNBytesPerFrame = d->frameBuffer->current()->getBytesPerFrame();
  d->handler->calculateFrameNumber();
  d->handler->configureBuffer( d->frameBuffer->current() );

  if( d->handler->m_uiWidth <= 0 || d->handler->m_uiHeight <= 0 || d->handler->m_iPixelFormat < 0 ||
      d->handler->m_uiBitsPerPixel == 0 || d->handler->m_uiTotalNumberFrames == 0 )
  {
    return false;
  }
  if( UInt( d->iCurrFrameNum ) >= d->handler->m_uiTotalNumberFrames )
  {
    d->iCurrFrameNum = 0;
  }
  Int currFrameNum = d->iCurrFrameNum;
  d->iCurrFrameNum = -1;
  seekInput( currFrameNum );
  return true;
}

Void PlaYUVerStream::close()
{
  if( !d->isInit )
    return;

  d->handler->closeHandler();
  d->handler->Delete();

  delete d->frameBuffer;

  d->bLoadAll = false;
  d->isInit = false;
}

String PlaYUVerStream::getFileName()
{
  return d->cFilename;
}

Bool PlaYUVerStream::isNative()
{
  return d->handler->m_bNative;
}

UInt PlaYUVerStream::getFrameNum()
{
  return d->handler->m_uiTotalNumberFrames;
}
UInt PlaYUVerStream::getWidth() const
{
  return d->handler->m_uiWidth;
}
UInt PlaYUVerStream::getHeight() const
{
  return d->handler->m_uiHeight;
}
Int PlaYUVerStream::getEndianess() const
{
  return d->handler->m_iEndianness;
}
Double PlaYUVerStream::getFrameRate()
{
  return d->handler->m_dFrameRate;
}

Int PlaYUVerStream::getCurrFrameNum()
{
  return d->iCurrFrameNum;
}

Void PlaYUVerStream::getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness,
                                UInt& rFrameRate )
{
  if( d->isInit )
  {
    rWidth = d->handler->m_uiWidth;
    rHeight = d->handler->m_uiHeight;
    rInputFormat = d->handler->m_iPixelFormat;
    rBitsPerPel = d->handler->m_uiBitsPerPixel;
    rEndianness = d->handler->m_iEndianness;
    rFrameRate = d->handler->m_dFrameRate;
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
  if( d->bLoadAll || !d->isInput )
    return;

  try
  {
    d->frameBuffer->increase( d->handler->m_uiTotalNumberFrames );
  }
  catch( PlaYUVerFailure& e )
  {
    close();
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot allocated frame buffer for the whole stream" );
  }

  seekInput( 0 );
  for( UInt i = 2; i < d->frameBuffer->size(); i++ )
  {
    readFrame( d->frameBuffer->frame( i ) );
  }
  d->bLoadAll = true;
  d->iCurrFrameNum = 0;
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
  if( !d->isInit || !d->isInput || d->handler->m_uiCurrFrameFileIdx >= d->handler->m_uiTotalNumberFrames )
    return false;

  if( d->bLoadAll )
    return true;

  if( !d->handler->read( frame ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot read frame from stream" );
    return false;
  }
  return true;
}

Void PlaYUVerStream::writeFrame()
{
  writeFrame( d->frameBuffer->current() );
}

Void PlaYUVerStream::writeFrame( PlaYUVerFrame* pcFrame )
{
  if( !d->handler->write( pcFrame ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot write frame into the stream" );
  }
  return;
}

Bool PlaYUVerStream::saveFrame( const String& filename )
{
  return saveFrame( filename, d->frameBuffer->current() );
}

Bool PlaYUVerStream::saveFrame( const String& filename, PlaYUVerFrame* saveFrame )
{
  PlaYUVerStream auxSaveStream;
  if( !auxSaveStream.open( filename, saveFrame->getWidth(), saveFrame->getHeight(), saveFrame->getPelFormat(),
                           saveFrame->getBitsPel(), PLAYUVER_LITTLE_ENDIAN, 1, false ) )
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

  if( d->iCurrFrameNum + 1 < Int64( d->handler->m_uiTotalNumberFrames ) )
  {
    d->frameBuffer->setNextFrame();
    d->iCurrFrameNum++;
  }
  else
  {
    bEndOfSeq = true;
  }
  return bEndOfSeq;
}

Void PlaYUVerStream::readNextFrame()
{
  readFrame( d->frameBuffer->next() );
}

Void PlaYUVerStream::readNextFrameFillRGBBuffer()
{
  readNextFrame();
  d->frameBuffer->next()->fillRGBBuffer();
  return;
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame( PlaYUVerFrame* pyuv_image )
{
  if( pyuv_image == NULL )
    pyuv_image = new PlaYUVerFrame( d->frameBuffer->current() );
  else
    pyuv_image->copyFrom( d->frameBuffer->current() );
  return pyuv_image;
}

PlaYUVerFrame* PlaYUVerStream::getCurrFrame()
{
  return d->frameBuffer->current();
}

Bool PlaYUVerStream::seekInputRelative( Bool bIsFoward )
{
  if( !d->isInit || !d->isInput )
    return false;

  Bool bRet = false;
  if( bIsFoward )
  {
    bRet = !setNextFrame();
    readFrame( d->frameBuffer->next() );
  }
  else
  {
    UInt64 newFrameNum = d->iCurrFrameNum - 1;
    bRet = seekInput( newFrameNum );
  }
  return bRet;
}

Bool PlaYUVerStream::seekInput( UInt64 new_frame_num )
{
  if( !d->isInit || new_frame_num >= d->handler->m_uiTotalNumberFrames || Int64( new_frame_num ) == d->iCurrFrameNum )
    return false;

  d->iCurrFrameNum = new_frame_num;

  if( d->bLoadAll )
  {
    d->frameBuffer->setIndex( d->iCurrFrameNum );
    return true;
  }

  if( !d->handler->seek( d->iCurrFrameNum ) )
  {
    throw PlaYUVerFailure( "PlaYUVerStream", "Cannot seek file into desired position" );
  }

  d->frameBuffer->setIndex( 0 );
  readFrame( d->frameBuffer->current() );
  if( d->handler->m_uiTotalNumberFrames > 1 )
    readFrame( d->frameBuffer->next() );

  return true;
}
