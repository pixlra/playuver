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
 * \file     CalypStream.h
 * \ingroup  CalypStreamGrp
 * \brief    Input stream handling
 */

#ifndef __CALYPSTREAM_H__
#define __CALYPSTREAM_H__

#include "CalypDefs.h"

class CalypFrame;
class CalypStreamHandlerIf;

typedef CalypStreamHandlerIf* ( *CreateStreamHandlerFn )( void );

typedef struct
{
  ClpString formatName;
  ClpString formatExt;
  std::vector<ClpString> getExts();
  ClpString formatPattern;
  CreateStreamHandlerFn formatFct;
} CalypStreamFormat;

typedef struct
{
  ClpString shortName;
  unsigned int uiWidth;
  unsigned int uiHeight;
} CalypStandardResolution;

/**
 * \class CalypStream
 * \ingroup CalypLibGrp CalypStreamGrp
 * \brief  Stream handling class
 */
class CalypStream
{
public:
  static std::vector<CalypStreamFormat> supportedReadFormats();
  static std::vector<CalypStreamFormat> supportedWriteFormats();

  static CreateStreamHandlerFn findStreamHandler( ClpString strFilename, bool bRead );

  static std::vector<CalypStandardResolution> stdResolutionSizes();

  CalypStream();
  ~CalypStream();

  ClpString getFormatName();
  ClpString getCodecName();

  bool open( ClpString filename, ClpString resolution, ClpString input_format, unsigned int bitsPel, int endianness, unsigned int frame_rate,
             bool bInput );
  bool open( ClpString filename, unsigned int width, unsigned int height, int input_format, unsigned int bitsPel, int endianness, unsigned int frame_rate,
             bool bInput );
  bool reload();
  void close();

  bool isNative();
  ClpString getFileName();
  unsigned int getFrameNum();
  unsigned int getWidth() const;
  unsigned int getHeight() const;
  int getEndianess() const;
  int getCurrFrameNum();
  double getFrameRate();
  void getFormat( unsigned int& rWidth, unsigned int& rHeight, int& rInputFormat, unsigned int& rBitsPerPel, int& rEndianness,
                  unsigned int& rFrameRate );

  void loadAll();

  void writeFrame();
  void writeFrame( CalypFrame* pcFrame );

  bool saveFrame( const ClpString& filename );
  static bool saveFrame( const ClpString& filename, CalypFrame* saveFrame );

  CalypFrame* getCurrFrame( CalypFrame* );

  // continuous read control
  bool setNextFrame();
  void readNextFrame();
  void readNextFrameFillRGBBuffer();
  CalypFrame* getCurrFrame();

  bool seekInputRelative( bool bIsFoward );
  bool seekInput( unsigned long new_frame_num );

  void getDuration( int* duration_array );

private:
  bool readFrame( CalypFrame* frame );

private:
  struct CalypStreamPrivate* d;
};

#endif  // __CALYPSTREAM_H__
