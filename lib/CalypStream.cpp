/*    This file is a part of Calyp project
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
 * \file     CalypStream.cpp
 * \brief    Input stream handling
 */

// Self
#include "CalypStream.h"

#include "CalypFrame.h"
#include "CalypStreamHandlerIf.h"
#include "LibMemory.h"
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

std::vector<CalypStreamFormat> CalypStream::supportedReadFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerRaw, Read );
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerPortableMap, Read );
//#ifdef USE_OPENCV
//  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerOpenCV, Read );
//#endif
#ifdef USE_FFMPEG
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerLibav, Read );
#endif
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStreamFormat> CalypStream::supportedWriteFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerRaw, Write );
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerPortableMap, Write );
#ifdef USE_FFMPEG
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerLibav, Write );
#endif
#ifdef USE_OPENCV
  APPEND_CALYP_SUPPORTED_FMT( StreamHandlerOpenCV, Write );
#endif
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStandardResolution> CalypStream::stdResolutionSizes()
{
#define REGIST_CALYP_STANDARD_RESOLUTION( name, width, height ) \
  stdResElement.shortName = name;                               \
  stdResElement.uiWidth = width;                                \
  stdResElement.uiHeight = height;                              \
  stdResList.push_back( stdResElement );

  std::vector<CalypStandardResolution> stdResList;
  CalypStandardResolution stdResElement;
  REGIST_CALYP_STANDARD_RESOLUTION( "CIF", 352, 288 );
  REGIST_CALYP_STANDARD_RESOLUTION( "VGA", 640, 480 );
  REGIST_CALYP_STANDARD_RESOLUTION( "WVGA", 832, 480 );
  REGIST_CALYP_STANDARD_RESOLUTION( "XVGA", 1024, 768 );
  REGIST_CALYP_STANDARD_RESOLUTION( "HD", 1280, 720 );
  REGIST_CALYP_STANDARD_RESOLUTION( "SXGA-", 1280, 900 );
  REGIST_CALYP_STANDARD_RESOLUTION( "SXGA", 1280, 1024 );
  REGIST_CALYP_STANDARD_RESOLUTION( "WSXGA", 1440, 900 );
  REGIST_CALYP_STANDARD_RESOLUTION( "FullHD", 1920, 1080 );
  REGIST_CALYP_STANDARD_RESOLUTION( "WQXGA", 2560, 1600 );
  REGIST_CALYP_STANDARD_RESOLUTION( "UltraHD", 3840, 2160 );
  REGIST_CALYP_STANDARD_RESOLUTION( "8K", 8192, 4608 );
  return stdResList;
}

class CalypStreamBufferPrivate
{
private:
  std::vector<CalypFrame*> m_apcFrameBuffer;
  unsigned int m_uiIndex;

public:
  CalypStreamBufferPrivate( unsigned int size, unsigned int width, unsigned int height, int pelFormat, int bitsPixel )
  {
    for( unsigned int i = 0; i < size; i++ )
    {
      CalypFrame* pFrame = new CalypFrame( width, height, pelFormat, bitsPixel );
      m_apcFrameBuffer.push_back( pFrame );
    }
    m_uiIndex = 0;
  }

  ~CalypStreamBufferPrivate()
  {
    while( m_apcFrameBuffer.size() > 0 )
    {
      delete m_apcFrameBuffer.back();
      m_apcFrameBuffer.pop_back();
    }
  }
  void increase( unsigned int newSize )
  {
    for( unsigned int i = m_apcFrameBuffer.size(); i < newSize; i++ )
    {
      CalypFrame* pFrame = new CalypFrame( m_apcFrameBuffer.at( 0 ) );
      m_apcFrameBuffer.push_back( pFrame );
    }
  }
  unsigned int size() { return m_apcFrameBuffer.size(); }
  void setIndex( unsigned int i ) { m_uiIndex = i; }
  CalypFrame* frame( int i ) { return m_apcFrameBuffer.at( i ); }

  CalypFrame* current() { return m_apcFrameBuffer.at( m_uiIndex ); }
  CalypFrame* next() { return m_apcFrameBuffer.at( nextIndex() ); }
  void setNextFrame() { m_uiIndex = nextIndex(); }

private:
  inline int nextIndex() { return m_uiIndex + 1 >= m_apcFrameBuffer.size() ? 0 : m_uiIndex + 1; }
  inline int prevIndex() { return m_uiIndex - 1 < 0 ? m_apcFrameBuffer.size() - 1 : m_uiIndex - 1; }
};

struct CalypStreamPrivate
{
  bool isInit;
  bool isInput;

  CalypStreamHandlerIf* handler;
  CreateStreamHandlerFn pfctCreateHandler;

  CalypStreamBufferPrivate* frameBuffer;

  ClpString cFilename;
  long long int iCurrFrameNum;
  bool bLoadAll;

  CalypStreamPrivate()
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

std::vector<ClpString> CalypStreamFormat::getExts()
{
  std::vector<ClpString> arrayExt;
  ClpString::size_type prev_pos = 0, pos = 0;
  while( ( pos = formatExt.find( ',', pos ) ) != ClpString::npos )
  {
    ClpString substring( formatExt.substr( prev_pos, pos - prev_pos ) );
    arrayExt.push_back( substring );
    prev_pos = ++pos;
  }
  arrayExt.push_back( formatExt.substr( prev_pos, pos - prev_pos ) );  // Last word
  return arrayExt;
}

CreateStreamHandlerFn CalypStream::findStreamHandler( ClpString strFilename, bool bRead )
{
  ClpString currExt = strFilename.substr( strFilename.find_last_of( "." ) + 1 );
  currExt = clpLowercase( currExt );

  std::vector<CalypStreamFormat> supportedFmts;
  if( bRead )
  {
    supportedFmts = CalypStream::supportedReadFormats();
  }
  else
  {
    supportedFmts = CalypStream::supportedWriteFormats();
  }
  for( unsigned int i = 0; i < supportedFmts.size(); i++ )
  {
    std::vector<ClpString> arrayExt = supportedFmts[i].getExts();
    for( std::vector<ClpString>::iterator e = arrayExt.begin(); e != arrayExt.end(); ++e )
    {
      if( currExt != "" && currExt == *e )
      {
        return supportedFmts[i].formatFct;
      }
      else if( strFilename.find( *e ) != ClpString::npos )
      {
        return supportedFmts[i].formatFct;
      }
    }
  }

#ifdef USE_FFMPEG
  return &StreamHandlerLibav::Create;
#else
  return &StreamHandlerRaw::Create;
#endif
}

CalypStream::CalypStream()
    : d( new CalypStreamPrivate )
{
}

CalypStream::~CalypStream()
{
  close();
}

ClpString CalypStream::getFormatName()
{
  return !d->handler ? "" : d->handler->getFormatName();
}
ClpString CalypStream::getCodecName()
{
  return !d->handler ? "" : d->handler->getCodecName();
}

bool CalypStream::open( ClpString filename, ClpString resolution, ClpString input_format_name, unsigned int bitsPel, int endianness,
                        unsigned int frame_rate, bool bInput )
{
  unsigned int width = 0;
  unsigned int height = 0;
  int input_format = -1;

  if( resolution.size() > 0 )
  {
    sscanf( resolution.c_str(), "%ux%u", &width, &height );
    if( width <= 0 || height <= 0 )
    {
      return false;
    }
  }
  for( unsigned int i = 0; i < CalypFrame::supportedPixelFormatListNames().size(); i++ )
  {
    if( clpLowercase( CalypFrame::supportedPixelFormatListNames()[i] ) == clpLowercase( input_format_name ) )
    {
      input_format = i;
      break;
    }
  }
  return open( filename, width, height, input_format, bitsPel, endianness, frame_rate, bInput );
}

bool CalypStream::open( ClpString filename, unsigned int width, unsigned int height, int input_format, unsigned int bitsPel, int endianness,
                        unsigned int frame_rate, bool bInput )
{
  if( d->isInit )
  {
    close();
  }
  d->isInit = false;
  d->isInput = bInput;

  d->pfctCreateHandler = CalypStream::findStreamHandler( filename, d->isInput );
  if( !d->pfctCreateHandler )
  {
    throw CalypFailure( "CalypStream", "Invalid handler" );
  }

  d->handler = d->pfctCreateHandler();

  if( !d->handler )
  {
    throw CalypFailure( "CalypStream", "Cannot create handler" );
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
    //throw CalypFailure( "CalypStream", "Incorrect configuration: width, height or pixel format" );
    return d->isInit;
  }

  // Keep past, current and future frames
  try
  {
    d->frameBuffer = new CalypStreamBufferPrivate( d->isInput ? 3 : 1, d->handler->m_uiWidth, d->handler->m_uiHeight,
                                                   d->handler->m_iPixelFormat, d->handler->m_uiBitsPerPixel );
  }
  catch( CalypFailure& e )
  {
    close();
    throw CalypFailure( "CalypStream", "Cannot allocated frame buffer" );
    return d->isInit;
  }

  d->handler->m_uiNBytesPerFrame = d->frameBuffer->current()->getBytesPerFrame();

  // Some handlers need to know how long is a frame to get frame number
  d->handler->calculateFrameNumber();

  if( d->isInput && d->handler->m_uiTotalNumberFrames == 0 )
  {
    close();
    throw CalypFailure( "CalypStream", "Incorrect configuration: less than one frame" );
    return d->isInit;
  }

  if( !d->handler->configureBuffer( d->frameBuffer->current() ) )
  {
    close();
    throw CalypFailure( "CalypStream", "Cannot allocated buffers" );
    return d->isInit;
  }

  d->iCurrFrameNum = -1;
  d->isInit = true;

  seekInput( 0 );

  d->isInit = true;
  return d->isInit;
}

bool CalypStream::reload()
{
  d->handler->closeHandler();
  if( !d->handler->openHandler( d->cFilename, d->isInput ) )
  {
    throw CalypFailure( "CalypStream", "Cannot open stream " + d->cFilename + " with the " +
                                           ClpString( d->handler->m_pchHandlerName ) + " handler" );
  }
  d->handler->m_uiNBytesPerFrame = d->frameBuffer->current()->getBytesPerFrame();
  d->handler->calculateFrameNumber();
  d->handler->configureBuffer( d->frameBuffer->current() );

  if( d->handler->m_uiWidth <= 0 || d->handler->m_uiHeight <= 0 || d->handler->m_iPixelFormat < 0 ||
      d->handler->m_uiBitsPerPixel == 0 || d->handler->m_uiTotalNumberFrames == 0 )
  {
    return false;
  }
  if( (unsigned int)( d->iCurrFrameNum ) >= d->handler->m_uiTotalNumberFrames )
  {
    d->iCurrFrameNum = 0;
  }
  int currFrameNum = d->iCurrFrameNum;
  d->iCurrFrameNum = -1;
  seekInput( currFrameNum );
  return true;
}

void CalypStream::close()
{
  if( !d->isInit )
    return;

  d->handler->closeHandler();
  d->handler->Delete();

  delete d->frameBuffer;

  d->bLoadAll = false;
  d->isInit = false;
}

ClpString CalypStream::getFileName()
{
  return d->cFilename;
}

bool CalypStream::isNative()
{
  return d->handler->m_bNative;
}

unsigned int CalypStream::getFrameNum()
{
  return d->handler->m_uiTotalNumberFrames;
}
unsigned int CalypStream::getWidth() const
{
  return d->handler->m_uiWidth;
}
unsigned int CalypStream::getHeight() const
{
  return d->handler->m_uiHeight;
}
int CalypStream::getEndianess() const
{
  return d->handler->m_iEndianness;
}
double CalypStream::getFrameRate()
{
  return d->handler->m_dFrameRate;
}

int CalypStream::getCurrFrameNum()
{
  return d->iCurrFrameNum;
}

void CalypStream::getFormat( unsigned int& rWidth, unsigned int& rHeight, int& rInputFormat, unsigned int& rBitsPerPel, int& rEndianness,
                             unsigned int& rFrameRate )
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
    rInputFormat = CLP_YUV420P;
    rBitsPerPel = 8;
    rEndianness = 0;
    rFrameRate = 30;
  }
}

void CalypStream::loadAll()
{
  if( d->bLoadAll || !d->isInput )
    return;

  try
  {
    d->frameBuffer->increase( d->handler->m_uiTotalNumberFrames );
  }
  catch( CalypFailure& e )
  {
    close();
    throw CalypFailure( "CalypStream", "Cannot allocated frame buffer for the whole stream" );
  }

  seekInput( 0 );
  for( unsigned int i = 2; i < d->frameBuffer->size(); i++ )
  {
    readFrame( d->frameBuffer->frame( i ) );
  }
  d->bLoadAll = true;
  d->iCurrFrameNum = 0;
}

void CalypStream::getDuration( int* duration_array )
{
  //   int hours, mins, secs = 0;
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

bool CalypStream::readFrame( CalypFrame* frame )
{
  if( !d->isInit || !d->isInput || d->handler->m_uiCurrFrameFileIdx >= d->handler->m_uiTotalNumberFrames )
    return false;

  if( d->bLoadAll )
    return true;

  if( !d->handler->read( frame ) )
  {
    throw CalypFailure( "CalypStream", "Cannot read frame from stream" );
    return false;
  }
  return true;
}

void CalypStream::writeFrame()
{
  writeFrame( d->frameBuffer->current() );
}

void CalypStream::writeFrame( CalypFrame* pcFrame )
{
  if( !d->handler->write( pcFrame ) )
  {
    throw CalypFailure( "CalypStream", "Cannot write frame into the stream" );
  }
  return;
}

bool CalypStream::saveFrame( const ClpString& filename )
{
  return saveFrame( filename, d->frameBuffer->current() );
}

bool CalypStream::saveFrame( const ClpString& filename, CalypFrame* saveFrame )
{
  CalypStream auxSaveStream;
  if( !auxSaveStream.open( filename, saveFrame->getWidth(), saveFrame->getHeight(), saveFrame->getPelFormat(),
                           saveFrame->getBitsPel(), CLP_LITTLE_ENDIAN, 1, false ) )
  {
    return false;
  }
  auxSaveStream.writeFrame( saveFrame );
  auxSaveStream.close();
  return true;
}

bool CalypStream::setNextFrame()
{
  bool bEndOfSeq = false;

  if( d->iCurrFrameNum + 1 < (long)( d->handler->m_uiTotalNumberFrames ) )
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

void CalypStream::readNextFrame()
{
  readFrame( d->frameBuffer->next() );
}

void CalypStream::readNextFrameFillRGBBuffer()
{
  readNextFrame();
  d->frameBuffer->next()->fillRGBBuffer();
  return;
}

CalypFrame* CalypStream::getCurrFrame( CalypFrame* pyuv_image )
{
  if( pyuv_image == NULL )
    pyuv_image = new CalypFrame( d->frameBuffer->current() );
  else
    pyuv_image->copyFrom( d->frameBuffer->current() );
  return pyuv_image;
}

CalypFrame* CalypStream::getCurrFrame()
{
  return d->frameBuffer->current();
}

bool CalypStream::seekInputRelative( bool bIsFoward )
{
  if( !d->isInit || !d->isInput )
    return false;

  bool bRet = false;
  if( bIsFoward )
  {
    bRet = !setNextFrame();
    readFrame( d->frameBuffer->next() );
  }
  else
  {
    unsigned long long int newFrameNum = d->iCurrFrameNum - 1;
    bRet = seekInput( newFrameNum );
  }
  return bRet;
}

bool CalypStream::seekInput( unsigned long new_frame_num )
{
  if( !d->isInit || new_frame_num >= d->handler->m_uiTotalNumberFrames || long( new_frame_num ) == d->iCurrFrameNum )
    return false;

  d->iCurrFrameNum = new_frame_num;

  if( d->bLoadAll )
  {
    d->frameBuffer->setIndex( d->iCurrFrameNum );
    return true;
  }

  if( !d->handler->seek( d->iCurrFrameNum ) )
  {
    throw CalypFailure( "CalypStream", "Cannot seek file into desired position" );
  }

  d->frameBuffer->setIndex( 0 );
  readFrame( d->frameBuffer->current() );
  if( d->handler->m_uiTotalNumberFrames > 1 )
    readFrame( d->frameBuffer->next() );

  return true;
}
