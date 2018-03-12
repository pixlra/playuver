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
 * \file     StreamHandlerRaw.cpp
 * \brief    interface for raw (yuv) streams
 */

#include "StreamHandlerRaw.h"

#include "CalypFrame.h"
#include "LibMemory.h"

#include <cstdio>

std::vector<CalypStreamFormat> StreamHandlerRaw::supportedReadFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw YUV Video", "yuv" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw Gray Video", "gray" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw RGB Video", "rgb" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStreamFormat> StreamHandlerRaw::supportedWriteFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerRaw::Create, "Raw Video", "yuv" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

bool StreamHandlerRaw::openHandler( ClpString strFilename, bool bInput )
{
  m_bIsInput = bInput;
  m_pFile = NULL;
  m_pFile = fopen( strFilename.c_str(), bInput ? "rb" : "wb" );
  if( m_pFile == NULL )
  {
    return false;
  }
  calculateFrameNumber();
  m_strFormatName = "YUV";
  m_strCodecName = "Raw Video";
  return true;
}

void StreamHandlerRaw::closeHandler()
{
  if( m_pFile )
    fclose( m_pFile );
  if( m_pStreamBuffer )
    freeMem1D( m_pStreamBuffer );
}

bool StreamHandlerRaw::configureBuffer( CalypFrame* pcFrame )
{
  return getMem1D<ClpByte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}

void StreamHandlerRaw::calculateFrameNumber()
{
  if( m_pFile && m_uiNBytesPerFrame > 0 )
  {
    fseek( m_pFile, 0, SEEK_END );
    unsigned long long int fileSize = ftell( m_pFile );
    fseek( m_pFile, 0, SEEK_SET );
    m_uiTotalNumberFrames = fileSize / m_uiNBytesPerFrame;
  }
}

bool StreamHandlerRaw::seek( unsigned long long int iFrameNum )
{
  if( m_bIsInput && m_pFile )
  {
    fseek( m_pFile, iFrameNum >= 0 ? iFrameNum * m_uiNBytesPerFrame : 0, SEEK_SET );
    m_uiCurrFrameFileIdx = iFrameNum;
    return true;
  }
  return false;
}

bool StreamHandlerRaw::read( CalypFrame* pcFrame )
{
  if( !m_pFile || !m_pStreamBuffer || m_uiNBytesPerFrame == 0 )
    return false;
  unsigned long long int processed_bytes = fread( m_pStreamBuffer, sizeof( ClpByte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  m_uiCurrFrameFileIdx++;
  pcFrame->frameFromBuffer( m_pStreamBuffer, m_iEndianness );
  return true;
}

bool StreamHandlerRaw::write( CalypFrame* pcFrame )
{
  pcFrame->frameToBuffer( m_pStreamBuffer, m_iEndianness );
  unsigned long long int processed_bytes = fwrite( m_pStreamBuffer, sizeof( ClpByte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  return true;
}
