/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     PlaYUVerFramePixelFormats.h
 * \brief    Handling the pixel formats definition
 */


#ifndef __PLAYUVERFRAMEPIXELFORMATS_H__
#define __PLAYUVERFRAMEPIXELFORMATS_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

template<typename T>
Void yuvToRgb( T iY, T iU, T iV, T &iR, T &iG, T &iB )
{
  iR = iY + 1402 * iV / 1000;
  iG = iY - ( 101004 * iU + 209599 * iV ) / 293500;
  iB = iY + 1772 * iU / 1000;

  if( iR < 0 )
    iR = 0;
  if( iG < 0 )
    iG = 0;
  if( iB < 0 )
    iB = 0;

  if( iR > 255 )
    iR = 255;
  if( iG > 255 )
    iG = 255;
  if( iB > 255 )
    iB = 255;
}

template<typename T>
Void rgbToYuv( T iR, T iG, T iB, T &iY, T &iU, T &iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

inline Int pelRed( UInt rgb )             // get red part of RGB
{
  return ( ( rgb >> 16 ) & 0xff );
}

inline Int pelGreen( UInt rgb )           // get green part of RGB
{
  return ( ( rgb >> 8 ) & 0xff );
}

inline Int pelBlue( UInt rgb )            // get blue part of RGB
{
  return ( rgb & 0xff );
}

inline Int pelAlpha( UInt rgb )           // get alpha part of RGBA
{
  return rgb >> 24;
}

inline UInt pelRgb( Int r, Int g, Int b )  // set RGB value
{
  return ( 0xffu << 24 ) | ( ( r & 0xff ) << 16 ) | ( ( g & 0xff ) << 8 ) | ( b & 0xff );
}

struct structPlaYUVerFramePelFormat
{
  const Char* name;
  Int colorSpace;
  UInt numberChannels;
  Int ratioChromaWidth;
  Int ratioChromaHeight;
  Void (*frameFromBuffer)( Pel *in, Pel*** out, UInt width, UInt height );
  Void (*bufferFromFrame)( Pel ***in, Pel* out, UInt width, UInt height );
  Void (*fillRGBbuffer)( Pel*** in, UChar* out, UInt width, UInt height );
  Int ffmpegPelFormat;
};

typedef struct structPlaYUVerFramePelFormat PlaYUVerFramePelFormat;

#define PLAYUVER_NUMBER_FORMATS 7
extern PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS];

}  // NAMESPACE

#endif // __PLAYUVERFRAMEPIXELFORMATS_H__
