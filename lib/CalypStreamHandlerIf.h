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
 * \file     CalypStreamHandlerIf.h
 * \brief    Abstract class for stream handling
 */

#ifndef __CALYPSTREAMHANDLERIF_H__
#define __CALYPSTREAMHANDLERIF_H__

#include "CalypStream.h"

#define INI_REGIST_CALYP_SUPPORTED_FMT        \
  std::vector<CalypStreamFormat> formatsList; \
  CalypStreamFormat formatElem;

#define END_REGIST_CALYP_SUPPORTED_FMT return formatsList;

#define REGIST_CALYP_SUPPORTED_FMT( handler, name, ext ) \
  formatElem.formatName = name;                          \
  formatElem.formatExt = clpLowercase( ext );            \
  formatElem.formatFct = handler;                        \
  formatsList.push_back( formatElem );

#define REGIST_CALYP_SUPPORTED_ABSTRACT_FMT( handler, name, pattern ) \
  formatElem.formatPattern = pattern;                                 \
  REGIST_CALYP_SUPPORTED_FMT( handler, name, "" )

#define APPEND_CALYP_SUPPORTED_FMT( class_name, fct )                                \
  {                                                                                  \
    std::vector<CalypStreamFormat> new_fmts = class_name::supported##fct##Formats(); \
    formatsList.insert( formatsList.end(), new_fmts.begin(), new_fmts.end() );       \
  }

#define REGISTER_CALYP_STREAM_HANDLER( X )                       \
public:                                                          \
  static CalypStreamHandlerIf* Create() { return new X(); }      \
  void Delete() { delete this; }                                 \
  static std::vector<CalypStreamFormat> supportedReadFormats();  \
  static std::vector<CalypStreamFormat> supportedWriteFormats(); \
  // static int checkforSupportedFile( String, bool );

/**
 * \class CalypStreamHandlerIf
 * \ingroup  CalypStreamGrp
 * \brief    Abstract class for stream handling
 */
class CalypStreamHandlerIf
{
  friend class CalypStream;

public:
  CalypStreamHandlerIf()
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
  virtual ~CalypStreamHandlerIf() {}
  virtual void Delete() = 0;

  virtual bool openHandler( ClpString strFilename, bool bInput ) = 0;
  virtual void closeHandler() = 0;
  virtual bool configureBuffer( CalypFrame* pcFrame ) = 0;
  virtual bool seek( unsigned long long int iFrameNum ) = 0;
  virtual bool read( CalypFrame* pcFrame ) = 0;
  virtual bool write( CalypFrame* pcFrame ) = 0;

  virtual void calculateFrameNumber(){};

  ClpString getFormatName() { return m_strFormatName; }
  ClpString getCodecName() { return m_strCodecName; }

protected:
  const char* m_pchHandlerName;

  ClpString m_strFormatName;
  ClpString m_strCodecName;
  bool m_bIsInput;
  bool m_bNative;
  unsigned long m_uiCurrFrameFileIdx;
  ClpString m_cFilename;
  unsigned int m_uiWidth;
  unsigned int m_uiHeight;
  int m_iPixelFormat;
  unsigned int m_uiBitsPerPixel;
  int m_iEndianness;
  double m_dFrameRate;
  unsigned long m_uiTotalNumberFrames;
  ClpByte* m_pStreamBuffer;
  unsigned long m_uiNBytesPerFrame;
};

#endif  // __CALYPSTREAMHANDLERIF_H__
