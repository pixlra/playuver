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
 * \file     PlaYUVerStreamHandlerIf.h
 * \brief    Abstract class for stream handling
 */

#ifndef __PLAYUVERSTREAMHANDLERIF_H__
#define __PLAYUVERSTREAMHANDLERIF_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerStream.h"
#include "PlaYUVerFrame.h"

#define REGISTER_STREAM_HANDLER(X) \
  public: \
    static PlaYUVerStreamHandlerIf* Create() { return new X(); } \
    void Delete() { delete this; }

/**
 * \class PlaYUVerStreamHandlerIf
 * \ingroup  PlaYUVerLib_Stream
 * \brief    Abstract class for stream handling
 */
class PlaYUVerStreamHandlerIf
{
  friend class PlaYUVerStream;
  public:
  PlaYUVerStreamHandlerIf()
  {
    m_bIsInput = true;
    m_uiWidth = 0;
    m_uiHeight = 0;
    m_iPixelFormat = -1;
    m_uiBitsPerPixel = 8;
    m_iEndianness = -1;
    m_dFrameRate = 30;
  }
  virtual ~PlaYUVerStreamHandlerIf()
  {
  }

  virtual Bool openHandler( String strFilename, Bool bInput ) = 0;
  virtual Void closeHandler() = 0;
  virtual Bool configureBuffer( PlaYUVerFrame* pcFrame ) = 0;
  virtual UInt64 calculateFrameNumber() = 0;
  virtual Bool seek( UInt64 iFrameNum ) = 0;
  virtual Bool read( PlaYUVerFrame* pcFrame ) = 0;
  virtual Bool write( PlaYUVerFrame* pcFrame ) = 0;

  Void setBytesPerFrame( UInt64 uiNBytes )
  {
    m_uiNBytesPerFrame = uiNBytes;
  }
  String getFormatName()
  {
    return m_strFormatName;
  }
  String getCodecName()
  {
    return m_strCodecName;
  }

protected:
  Bool m_bIsInput;
  String m_cFilename;
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  UInt m_uiBitsPerPixel;
  Int m_iEndianness;
  Double m_dFrameRate;

  Byte* m_pStreamBuffer;
  UInt64 m_uiNBytesPerFrame;
  String m_strFormatName;
  String m_strCodecName;
};

#endif // __PLAYUVERSTREAMHANDLERIF_H__

