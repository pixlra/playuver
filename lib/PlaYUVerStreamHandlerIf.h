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
 * \file     PlaYUVerStreamHandlerIf.h
 * \brief    Abstract class for stream handling
 */

#ifndef __PLAYUVERSTREAMHANDLERIF_H__
#define __PLAYUVERSTREAMHANDLERIF_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerStream.h"

class PlaYUVerFrame;

#define INI_REGIST_PLAYUVER_SUPPORTED_FMT           \
  std::vector<PlaYUVerSupportedFormat> formatsList; \
  PlaYUVerSupportedFormat formatElem;

#define END_REGIST_PLAYUVER_SUPPORTED_FMT return formatsList;

#define REGIST_PLAYUVER_SUPPORTED_FMT( handler, name, ext ) \
  formatElem.formatName = name;                             \
  formatElem.formatExt = lowercase( ext );                  \
  formatElem.formatFct = handler;                           \
  formatsList.push_back( formatElem );

#define REGIST_PLAYUVER_SUPPORTED_ABSTRACT_FMT( handler, name, pattern ) \
  formatElem.formatPattern = pattern;                                    \
  REGIST_PLAYUVER_SUPPORTED_FMT( handler, name, "" )

#define APPEND_PLAYUVER_SUPPORTED_FMT( class_name, fct )                                   \
  {                                                                                        \
    std::vector<PlaYUVerSupportedFormat> new_fmts = class_name::supported##fct##Formats(); \
    formatsList.insert( formatsList.end(), new_fmts.begin(), new_fmts.end() );             \
  }

#define REGISTER_STREAM_HANDLER( X )                                   \
public:                                                                \
  static PlaYUVerStreamHandlerIf* Create() { return new X(); }         \
  void Delete() { delete this; }                                       \
  static std::vector<PlaYUVerSupportedFormat> supportedReadFormats();  \
  static std::vector<PlaYUVerSupportedFormat> supportedWriteFormats(); \
  // static Int checkforSupportedFile( String, Bool );

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
      : m_bIsInput( true )
      , m_bNative( true )
      , m_uiCurrFrameFileIdx( 0 )
      , m_uiWidth( 0 )
      , m_uiHeight( 0 )
      , m_iPixelFormat( -1 )
      , m_uiBitsPerPixel( 8 )
      , m_iEndianness( -1 )
      , m_dFrameRate( 30 )
      , m_uiTotalNumberFrames( 0 )
      , m_pStreamBuffer( NULL )
      , m_uiNBytesPerFrame( 0 )
  {
  }
  virtual ~PlaYUVerStreamHandlerIf() {}
  virtual void Delete() = 0;

  virtual Bool openHandler( String strFilename, Bool bInput ) = 0;
  virtual Void closeHandler() = 0;
  virtual Bool configureBuffer( PlaYUVerFrame* pcFrame ) = 0;
  virtual Bool seek( UInt64 iFrameNum ) = 0;
  virtual Bool read( PlaYUVerFrame* pcFrame ) = 0;
  virtual Bool write( PlaYUVerFrame* pcFrame ) = 0;

  virtual Void calculateFrameNumber(){};

  String getFormatName() { return m_strFormatName; }
  String getCodecName() { return m_strCodecName; }

protected:
  const Char* m_pchHandlerName;

  String m_strFormatName;
  String m_strCodecName;
  Bool m_bIsInput;
  Bool m_bNative;
  UInt64 m_uiCurrFrameFileIdx;
  String m_cFilename;
  UInt m_uiWidth;
  UInt m_uiHeight;
  Int m_iPixelFormat;
  UInt m_uiBitsPerPixel;
  Int m_iEndianness;
  Double m_dFrameRate;
  UInt64 m_uiTotalNumberFrames;
  Byte* m_pStreamBuffer;
  UInt64 m_uiNBytesPerFrame;
};

#endif  // __PLAYUVERSTREAMHANDLERIF_H__
