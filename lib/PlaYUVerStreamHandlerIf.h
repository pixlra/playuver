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
 * \file     PlaYUVerStreamHandlerIf.h
 * \brief    Abstract class for stream handling
 */

#ifndef __PLAYUVERSTREAMHANDLERIF_H__
#define __PLAYUVERSTREAMHANDLERIF_H__

#include "PlaYUVerDefs.h"
// #include <iostream>
// #include <fstream>
// #include <cstdio>

namespace plaYUVer
{

/**
 * \class PlaYUVerStreamHandlerIf
 * \ingroup  PlaYUVerLib PlaYUVerLib_Stream
 * \brief    Abstract class for stream handling
 */
class PlaYUVerStreamHandlerIf
{
public:
  PlaYUVerStreamHandlerIf()
  {
    m_bIsInput = true;
  }
  ~PlaYUVerStreamHandlerIf()
  {
  }
  virtual Bool openHandler( std::string strFilename, Bool bInput ) = 0;
  virtual UInt64 calculateFrameNumber() = 0;
  virtual Void closeHandler() = 0;
  virtual Bool read( Byte* pchBuffer ) = 0;
  virtual Bool write( Byte* pchBuffer ) = 0;
  virtual Bool seek( UInt64 iFrameNum ) = 0;

  virtual Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness, Double& rFrameRate )
  {
    return;
  }
  Void setBytesPerFrame( UInt64 uiNBytes )
  {
    m_uiNBytesPerFrame = uiNBytes;
  }
  std::string getFormatName()
  {
    return m_strFormatName;
  }
  std::string getCodecName()
  {
    return m_strCodecName;
  }
protected:
  Bool m_bIsInput;
  UInt64 m_uiNBytesPerFrame;
  std::string m_strFormatName;
  std::string m_strCodecName;
};


}  // NAMESPACE

#endif // __PLAYUVERSTREAMHANDLERIF_H__

