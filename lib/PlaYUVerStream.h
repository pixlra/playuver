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
 * \file     PlaYUVerStream.h
 * \brief    Input stream handling
 */

#ifndef __PLAYUVERSTREAM_H__
#define __PLAYUVERSTREAM_H__

#include "PlaYUVerDefs.h"

class PlaYUVerFrame;
class PlaYUVerStreamPrivate;
class PlaYUVerStreamHandlerIf;

typedef PlaYUVerStreamHandlerIf* ( *CreateStreamHandlerFn )( void );

typedef struct
{
  String formatName;
  String formatExt;
  std::vector<String> getExts();
  String formatPattern;
  CreateStreamHandlerFn formatFct;
} PlaYUVerSupportedFormat;

typedef struct
{
  String shortName;
  UInt uiWidth;
  UInt uiHeight;
} PlaYUVerStdResolution;

/**
 * \class PlaYUVerStream
 * \ingroup  PlaYUVerLib PlaYUVerLib_Stream
 * \brief  Stream handling class
 */
class PlaYUVerStream
{
public:
  static std::vector<PlaYUVerSupportedFormat> supportedReadFormats();
  static std::vector<PlaYUVerSupportedFormat> supportedWriteFormats();

  static CreateStreamHandlerFn findStreamHandler( String strFilename, bool bRead );

  static std::vector<PlaYUVerStdResolution> stdResolutionSizes();

  PlaYUVerStream();
  ~PlaYUVerStream();

  enum PlaYUVerStreamErrors
  {
    NO_STREAM_ERROR = 0,
    READING,
    WRITING,
    LAST_FRAME,
    END_OF_SEQ,
  };

  String getFormatName();
  String getCodecName();

  Bool open( String filename, String resolution, String input_format, UInt bitsPel, Int endianness, UInt frame_rate,
             Bool bInput );
  Bool open( String filename, UInt width, UInt height, Int input_format, UInt bitsPel, Int endianness, UInt frame_rate,
             Bool bInput );
  Bool reload();
  Void close();

  Bool isNative();
  String getFileName();
  UInt getFrameNum();
  UInt getWidth() const;
  UInt getHeight() const;
  Int getEndianess() const;
  Int getCurrFrameNum();
  Double getFrameRate();
  Void getFormat( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBitsPerPel, Int& rEndianness,
                  UInt& rFrameRate );

  Void loadAll();

  Void writeFrame();
  Void writeFrame( PlaYUVerFrame* pcFrame );

  Bool saveFrame( const String& filename );
  static Bool saveFrame( const String& filename, PlaYUVerFrame* saveFrame );

  PlaYUVerFrame* getCurrFrame( PlaYUVerFrame* );

  // continuous read control
  Bool setNextFrame();
  Void readNextFrame();
  Void readNextFrameFillRGBBuffer();
  PlaYUVerFrame* getCurrFrame();

  Bool seekInputRelative( Bool bIsFoward );
  Bool seekInput( UInt64 new_frame_num );

  Void getDuration( Int* duration_array );

private:
  Bool readFrame( PlaYUVerFrame* frame );

private:
  PlaYUVerStreamPrivate* d;
};

#endif  // __PLAYUVERSTREAM_H__
