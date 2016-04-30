/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     StreamHandlerPortableMap.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Interface with opencv lib
 */

#include <cstdio>
#include <cmath>
#include "StreamHandlerPortableMap.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"


Bool StreamHandlerPortableMap::openHandler( std::string strFilename, Bool bInput )
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
    while( fgets( line, 100, m_pFile ) && line[0] == '#' );
    sscanf( line,"P%d", &m_iMagicNumber );
    while( fgets( line, 100, m_pFile ) && line[0] == '#' );
    sscanf( line,"%u %u", &m_uiWidth, &m_uiHeight );

    if( m_iMagicNumber == 1 || m_iMagicNumber == 4 )
    {
      m_uiBitsPerPixel = 1;
      m_iMaxValue = 1;
      m_iPixelFormat = PlaYUVerFrame::GRAY;
    }
    else
    {
      while( fgets( line, 100, m_pFile ) && line[0] == '#' );
      sscanf( line,"%d", &m_iMaxValue );
      m_uiBitsPerPixel = log( m_iMaxValue + 1 )/log( 2 );
      m_iPixelFormat = m_iMagicNumber == 2 || m_iMagicNumber == 5 ? PlaYUVerFrame::GRAY :  PlaYUVerFrame::RGB24;
    }
  }
  else
  {
    m_iMaxValue = ( 1 << m_uiBitsPerPixel ) - 1;
    if( m_uiBitsPerPixel == 1 )
    {
      m_iMagicNumber = 4;
    }
    else if( m_iPixelFormat == PlaYUVerFrame::GRAY )
    {
      m_iMagicNumber = 5;
    }
    else if( m_iPixelFormat == PlaYUVerFrame::RGB24 )
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
  return true;
}

Void StreamHandlerPortableMap::closeHandler()
{
  if( m_pFile )
    fclose( m_pFile );

  if( m_pStreamBuffer )
    freeMem1D( m_pStreamBuffer );
}

Bool StreamHandlerPortableMap::configureBuffer( PlaYUVerFrame* pcFrame )
{
  return getMem1D<Byte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}


UInt64 StreamHandlerPortableMap::calculateFrameNumber()
{
  return 1;
}

Bool StreamHandlerPortableMap::seek( UInt64 iFrameNum )
{
  return true;
}

Bool StreamHandlerPortableMap::read( PlaYUVerFrame* pcFrame )
{
  UInt64 processed_bytes = fread( m_pStreamBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  pcFrame->frameFromBuffer( m_pStreamBuffer );
  return true;
}

Bool StreamHandlerPortableMap::write( PlaYUVerFrame* pcFrame )
{
  fseek( m_pFile, 0, SEEK_SET );
  fprintf( m_pFile, "P%d\n%d %d\n", m_iMagicNumber, m_uiWidth, m_uiHeight );
  if( m_iMagicNumber > 4 )
  {
    fprintf( m_pFile, "%d\n", m_iMaxValue );
  }
  pcFrame->frameToBuffer( m_pStreamBuffer );
  UInt64 processed_bytes = fwrite( m_pStreamBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  return true;
}


