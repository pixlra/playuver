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
 * \ingroup  PlaYUVerLib
 * \brief    Input stream handling
 */

#include "config.h"
#include <cstdio>
#include <vector>
#include "LibMemory.h"
#include "PlaYUVerStream.h"
#include "PlaYUVerStreamHandlerIf.h"
#include "StreamHandlerRaw.h"
#include "StreamHandlerPortableMap.h"

#ifdef USE_FFMPEG
#include "StreamHandlerLibav.h"
#endif
#ifdef USE_OPENCV
#include "StreamHandlerOpenCV.h"
#endif

namespace plaYUVer
{

std::vector<PlaYUVerSupportedFormat> PlaYUVerStream::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &PlaYUVerRawHandler::Create, "Raw Video", "yuv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &PlaYUVerRawHandler::Create, "Raw Video", "gray" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable BitMap ", "pbm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable PixMap ", "ppm" );
#ifdef USE_FFMPEG
  APPEND_PLAYUVER_SUPPORTED_FMT( StreamHandlerLibav, Read );
#endif
//#ifdef USE_OPENCV
//  APPEND_PLAYUVER_SUPPORTED_FMT( LibOpenCVHandler );
//#endif
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> PlaYUVerStream::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( &PlaYUVerRawHandler::Create, "Raw Video", "yuv" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable BitMap ", "pbm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable PixMap ", "ppm" );
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

#define REGIST_PLAYUVER_STANDARD_RESOLUTION( name, width, height) \
    stdResElement.shortName = name; \
    stdResElement.uiWidth = width; \
    stdResElement.uiHeight = height; \
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


CreateStreamHandlerFn PlaYUVerStream::findStreamHandler( std::string strFilename, bool bRead )
{
  std::string currExt = strFilename.substr( strFilename.find_last_of( "." ) + 1 );
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
    if( supportedFmts[i].formatExt == currExt )
    {
      return supportedFmts[i].formatFct;
    }
  }
  return &PlaYUVerRawHandler::Create;
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
  m_pcCurrFrame = NULL;
  m_pcNextFrame = NULL;
  m_ppcFrameBuffer = NULL;
  m_uiFrameBufferSize = 2;
  m_uiFrameBufferIndex = 0;
  m_uiCurrFrameFileIdx = 0;
}

PlaYUVerStream::~PlaYUVerStream()
{
  close();
}

std::string PlaYUVerStream::getFormatName()
{
  return !m_pcHandler ? "" : m_pcHandler->getFormatName();
}
std::string PlaYUVerStream::getCodecName()
{
  return !m_pcHandler ? "" : m_pcHandler->getCodecName();
}

Bool PlaYUVerStream::open( std::string filename, std::string resolution, std::string input_format_name, UInt bitsPel, Int endianness, UInt frame_rate, Bool bInput )
{
  UInt width;
  UInt height;
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

Bool PlaYUVerStream::open( std::string filename, UInt width, UInt height, Int input_format, UInt bitsPel, Int endianness, UInt frame_rate, Bool bInput )
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
    close();
    throw "[PlaYUVerStream] Invalid handler";
  }

  m_pcHandler = m_pfctCreateHandler();

  if( !m_pcHandler )
  {
    close();
    throw "[PlaYUVerStream] Invalid handler";
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
    throw "[PlaYUVerStream] Cannot open file";
    return m_bInit;
  }


  if( m_pcHandler->m_uiWidth <= 0 || m_pcHandler->m_uiHeight <= 0 || m_pcHandler->m_iPixelFormat < 0 )
  {
    close();
    throw "[PlaYUVerStream] Incorrect configuration";
    return m_bInit;
  }

  m_uiFrameBufferSize = m_bIsInput ? 2 : 1;

  getMem1D<PlaYUVerFrame*>( &m_ppcFrameBuffer, m_uiFrameBufferSize );
  for( UInt i = 0; i < m_uiFrameBufferSize; i++ )
  {
    try
    {
      m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_pcHandler->m_uiWidth, m_pcHandler->m_uiHeight, m_pcHandler->m_iPixelFormat, m_pcHandler->m_uiBitsPerPixel, m_pcHandler->m_iEndianness );
    }
    catch( const char *msg )
    {
      close();
      throw "[PlaYUVerStream] Cannot allocated frame buffer";
      return m_bInit;
    }
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  m_pcHandler->setBytesPerFrame( m_pcCurrFrame->getBytesPerFrame() );

  if( m_bIsInput )
  {
    m_uiTotalFrameNum = m_pcHandler->calculateFrameNumber();
  }

  if( m_bIsInput && m_uiTotalFrameNum <= 0 )
  {
    close();
    throw "[PlaYUVerStream] Incorrect configuration: less than one frame";
    return m_bInit;
  }

  if( !m_pcHandler->configureBuffer( m_pcCurrFrame ) )
  {
    close();
    throw "[PlaYUVerStream] Cannot allocated memory";
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
    throw "[PlaYUVerStream] Cannot open file using FFmpeg libs";
  }
  m_pcHandler->setBytesPerFrame( m_pcCurrFrame->getBytesPerFrame() );
  m_uiTotalFrameNum = m_pcHandler->calculateFrameNumber();

  if( m_pcHandler->m_uiWidth <= 0 || m_pcHandler->m_uiHeight <= 0
    || m_pcHandler->m_iPixelFormat < 0 || m_pcHandler->m_uiBitsPerPixel == 0 || m_uiTotalFrameNum < 1 )
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

  m_bLoadAll = false;
  m_bInit = false;
}

std::string PlaYUVerStream::getFileName()
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
Double PlaYUVerStream::getFrameRate()
{
  return m_pcHandler->m_dFrameRate;
}

Int PlaYUVerStream::getCurrFrameNum()
{
  return m_iCurrFrameNum;
}

Void PlaYUVerStream::getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness, UInt& rFrameRate )
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
    m_ppcFrameBuffer[i] = new PlaYUVerFrame( m_pcHandler->m_uiWidth, m_pcHandler->m_uiHeight, m_pcHandler->m_iPixelFormat, m_pcHandler->m_uiBitsPerPixel );
    if( !m_ppcFrameBuffer[i] )
    {
      close();
    }
  }
  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
  m_iCurrFrameNum = -1;
  seekInput( 0 );
  for( UInt i = m_uiFrameBufferIndex + 1; i < m_uiFrameBufferSize; i++ )
  {
    m_pcNextFrame = m_ppcFrameBuffer[i];
    readFrame();
  }
  m_bLoadAll = true;
  m_uiFrameBufferIndex = 0;
  m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
  m_iCurrFrameNum = -1;
  setNextFrame();
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

Void PlaYUVerStream::readFrameFillRGBBuffer()
{
  readFrame();
  if( m_pcNextFrame )
    m_pcNextFrame->fillRGBBuffer();
  return;
}

Void PlaYUVerStream::readFrame()
{
  if( !m_bInit || !m_bIsInput || m_bLoadAll )
    return;

  if( m_uiCurrFrameFileIdx >= m_uiTotalFrameNum )
  {
    m_pcNextFrame = NULL;
    return;
  }

  if( !m_pcHandler->read( m_pcNextFrame ) )
  {
    throw "[PlaYUVerStream] Cannot read file";
    return;
  }

  m_uiCurrFrameFileIdx++;
  return;
}

Void PlaYUVerStream::writeFrame()
{
  writeFrame( m_pcCurrFrame );
}

Void PlaYUVerStream::writeFrame( PlaYUVerFrame *pcFrame )
{
  if( !m_pcHandler->write( pcFrame ) )
  {
    throw "[PlaYUVerStream] Cannot write into the file";
  }
  return;
}

Bool PlaYUVerStream::saveFrame( const std::string& filename )
{
  return saveFrame( filename, m_pcCurrFrame );
}

Bool PlaYUVerStream::saveFrame( const std::string& filename, PlaYUVerFrame *saveFrame )
{
  PlaYUVerStream auxSaveStream;
  if( !auxSaveStream.open( filename, saveFrame->getWidth(), saveFrame->getHeight(), saveFrame->getPelFormat(), saveFrame->getBitsPel(), saveFrame->getEndianness(), 1, false ) )
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
  if( m_pcNextFrame )
  {
    m_pcCurrFrame = m_pcNextFrame;
    m_iCurrFrameNum++;
    if( m_iCurrFrameNum + 1 < Int( m_uiTotalFrameNum ) )
    {
      m_uiFrameBufferIndex++;
      if( m_uiFrameBufferIndex == m_uiFrameBufferSize )
      {
        m_uiFrameBufferIndex = 0;
      }
      m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];
    }
    else
    {
      m_pcNextFrame = NULL;
    }
  }
  else
  {
    bEndOfSeq = true;
  }
  return bEndOfSeq;
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

PlaYUVerFrame* PlaYUVerStream::getNextFrame()
{
  return m_pcNextFrame;
}

Bool PlaYUVerStream::seekInputRelative( Bool bIsFoward )
{
  if( !m_bInit || !m_bIsInput )
    return false;

  Bool bRet = false;
  if( bIsFoward )
  {
    bRet = !setNextFrame();
    readFrame();
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

  if( !m_pcHandler->seek( new_frame_num ) )
  {
    throw "[PlaYUVerStream] Cannot write into the file";
  }

  m_uiCurrFrameFileIdx = new_frame_num;

  m_uiFrameBufferIndex = 0;
  m_pcCurrFrame = m_pcNextFrame = m_ppcFrameBuffer[m_uiFrameBufferIndex];

  readFrame();
  setNextFrame();
  if( m_uiTotalFrameNum > 1 )
    readFrame();
  return true;
}

}  // NAMESPACE
