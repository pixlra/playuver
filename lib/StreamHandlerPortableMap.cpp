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
 * \file     StreamHandlerPortableMap.cpp
 * \brief    Handling portable pixmap formats
 */

#include "StreamHandlerPortableMap.h"

#include "CalypFrame.h"
#include "LibMemory.h"

#include <cmath>
#include <cstdio>

std::vector<CalypStreamFormat> StreamHandlerPortableMap::supportedReadFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  //REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable BitMap ", "pbm" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable PixMap ", "ppm" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

std::vector<CalypStreamFormat> StreamHandlerPortableMap::supportedWriteFormats()
{
  INI_REGIST_CALYP_SUPPORTED_FMT;
  REGIST_CALYP_SUPPORTED_FMT( &StreamHandlerPortableMap::Create, "Portable GrayMap ", "pgm" );
  END_REGIST_CALYP_SUPPORTED_FMT;
}

bool StreamHandlerPortableMap::openHandler( ClpString strFilename, bool bInput )
{
  m_bIsInput = bInput;
  m_pFile = NULL;
  m_pFile = fopen( strFilename.c_str(), bInput ? "rb" : "wb" );
  if( m_pFile == NULL )
  {
    return false;
  }
  m_strFormatName = "PGM";
  m_strCodecName = "Raw Video";
  if( m_bIsInput )
  {
    char line[101];
    while( fgets( line, 100, m_pFile ) && line[0] == '#' )
      ;
    sscanf( line, "P%d", &m_iMagicNumber );
    while( fgets( line, 100, m_pFile ) && line[0] == '#' )
      ;
    sscanf( line, "%u %u", &m_uiWidth, &m_uiHeight );

    if( m_iMagicNumber == 1 || m_iMagicNumber == 4 )
    {
      m_uiBitsPerPixel = 1;
      m_iMaxValue = 1;
      m_iPixelFormat = CLP_GRAY;
    }
    else
    {
      while( fgets( line, 100, m_pFile ) && line[0] == '#' )
        ;
      sscanf( line, "%d", &m_iMaxValue );
      m_uiBitsPerPixel = log( m_iMaxValue + 1 ) / log( 2 );
      m_iPixelFormat = m_iMagicNumber == 2 || m_iMagicNumber == 5 ? CLP_GRAY : CLP_RGB24;
    }
  }
  else
  {
    m_iMaxValue = ( 1 << m_uiBitsPerPixel ) - 1;
    if( m_uiBitsPerPixel == 1 )
    {
      m_iMagicNumber = 4;
    }
    else if( m_iPixelFormat == CLP_GRAY )
    {
      m_iMagicNumber = 5;
    }
    else if( m_iPixelFormat == CLP_RGB24 )
    {
      m_iMagicNumber = 6;
    }
    else
    {
      closeHandler();
      return false;
    }
  }
  m_iEndianness = 0;
  m_dFrameRate = 1;
  m_uiTotalNumberFrames = 1;
  return true;
}

void StreamHandlerPortableMap::closeHandler()
{
  if( m_pFile )
    fclose( m_pFile );

  if( m_pStreamBuffer )
    freeMem1D( m_pStreamBuffer );
}

bool StreamHandlerPortableMap::configureBuffer( CalypFrame* pcFrame )
{
  return getMem1D<ClpByte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}

bool StreamHandlerPortableMap::seek( unsigned long long int iFrameNum )
{
  // m_uiCurrFrameFileIdx =
  return true;
}

bool StreamHandlerPortableMap::read( CalypFrame* pcFrame )
{
  unsigned long long int processed_bytes = fread( m_pStreamBuffer, sizeof( ClpByte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  pcFrame->frameFromBuffer( m_pStreamBuffer, CLP_BIG_ENDIAN );
  m_uiCurrFrameFileIdx++;
  return true;
}

bool StreamHandlerPortableMap::write( CalypFrame* pcFrame )
{
  fseek( m_pFile, 0, SEEK_SET );
  fprintf( m_pFile, "P%d\n%d %d\n", m_iMagicNumber, m_uiWidth, m_uiHeight );
  if( m_iMagicNumber > 4 )
  {
    fprintf( m_pFile, "%d\n", m_iMaxValue );
  }
  pcFrame->frameToBuffer( m_pStreamBuffer, CLP_BIG_ENDIAN );
  unsigned long long int processed_bytes = fwrite( m_pStreamBuffer, sizeof( ClpByte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  return true;
}
