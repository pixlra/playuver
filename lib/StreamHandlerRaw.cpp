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
 * \file     StreamHandlerRaw.cpp
 * \brief    Interface for raw (yuv) streams
 */

#include "StreamHandlerRaw.h"

#include <cstdio>

#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"

Bool StreamHandlerRaw::openHandler( String strFilename, Bool bInput )
{
  m_bIsInput = bInput;
  m_pFile = NULL;
  m_pFile = fopen( strFilename.c_str(), bInput ? "rb" : "wb" );
  if( m_pFile == NULL )
  {
    return false;
  }
  m_strFormatName = "YUV";
  m_strCodecName = "Raw Video";
  return true;
}

Void StreamHandlerRaw::closeHandler()
{
  if( m_pFile )
    fclose( m_pFile );
}

Bool StreamHandlerRaw::configureBuffer( PlaYUVerFrame* pcFrame )
{
  return getMem1D<Byte>( &m_pStreamBuffer, pcFrame->getBytesPerFrame() );
}

UInt64 StreamHandlerRaw::calculateFrameNumber()
{
  if( !m_pFile || m_uiNBytesPerFrame == 0 )
    return 0;
  fseek( m_pFile, 0, SEEK_END );
  UInt64 fileSize = ftell( m_pFile );
  fseek( m_pFile, 0, SEEK_SET );
  return ( fileSize / m_uiNBytesPerFrame );
}

Bool StreamHandlerRaw::seek( UInt64 iFrameNum )
{
  if( m_bIsInput && m_pFile )
  {
    fseek( m_pFile, iFrameNum >= 0 ? iFrameNum * m_uiNBytesPerFrame : 0, SEEK_SET );
    return true;
  }
  return false;
}

Bool StreamHandlerRaw::read( PlaYUVerFrame* pcFrame )
{
  UInt64 processed_bytes = fread( m_pStreamBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  pcFrame->frameFromBuffer( m_pStreamBuffer, m_iEndianness );
  return true;
}

Bool StreamHandlerRaw::write( PlaYUVerFrame* pcFrame )
{
  pcFrame->frameToBuffer( m_pStreamBuffer, m_iEndianness );
  UInt64 processed_bytes = fwrite( m_pStreamBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
  if( processed_bytes != m_uiNBytesPerFrame )
    return false;
  return true;
}
