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
 * \file     PlaYUVerRawHandler.h
 * \brief    Abstract class for stream handling
 */

#ifndef __PLAYUVERRAWHANDLER_H__
#define __PLAYUVERRAWHANDLER_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerStreamHandlerIf.h"

namespace plaYUVer
{

/**
 * \class PlaYUVerRawHandler
 * \ingroup  PlaYUVerLib PlaYUVerLib_Stream
 * \brief    Class to handle raw video format
 */
class PlaYUVerRawHandler: public PlaYUVerStreamHandlerIf
{
private:
  FILE* m_pFile; /**< The input file pointer >*/
public:

  Bool openHandler( std::string strFilename, Bool bInput )
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
  Void closeHandler()
  {
    if( m_pFile )
      fclose( m_pFile );
  }
  UInt64 calculateFrameNumber()
  {
    if( !m_pFile || m_uiNBytesPerFrame == 0 )
      return 0;
    fseek( m_pFile, 0, SEEK_END );
    UInt64 fileSize = ftell( m_pFile );
    fseek( m_pFile, 0, SEEK_SET );
    return ( fileSize / m_uiNBytesPerFrame );
  }
  Bool seek( UInt64 iFrameNum )
  {
    if( m_bIsInput && m_pFile )
    {
      fseek( m_pFile, iFrameNum >= 0 ? iFrameNum * m_uiNBytesPerFrame : 0, SEEK_SET );
      return true;
    }
    return false;
  }
  Bool read( Byte* pchBuffer )
  {
    UInt64 processed_bytes = fread( pchBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
    if( processed_bytes != m_uiNBytesPerFrame )
      return false;
    return true;
  }
  Bool write( Byte* pchBuffer )
  {
    UInt64 processed_bytes = fwrite( pchBuffer, sizeof( Byte ), m_uiNBytesPerFrame, m_pFile );
    if( processed_bytes != m_uiNBytesPerFrame )
      return false;
    return true;
  }
};


}  // NAMESPACE

#endif // __PLAYUVERRAWHANDLER_H__

