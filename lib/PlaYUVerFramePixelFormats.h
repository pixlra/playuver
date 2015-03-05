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
 * \file     PlaYUVerFramePixelFormats.h
 * \brief    Handling the pixel formats definition
 */

#ifndef __PLAYUVERFRAMEPIXELFORMATS_H__
#define __PLAYUVERFRAMEPIXELFORMATS_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

#define CHROMA_RESAMPLING( X ) ( ( ( X + 1 ) >> 1 ) << 1 )

#define CHROMARATIO( SIZE, RATIO ) ( RATIO > 1 ? ( ( SIZE + 1 ) / RATIO ) : SIZE )
#define CHROMASHIFT( SIZE, SHIFT ) ( SHIFT > 0 ? ( ( SIZE + 1 ) >> SHIFT ) : SIZE )

#define CLAMP(X) X = X < 0 ? 0 : X > 255 ? 255 : X;

#define YUV2RGB(  iY, iU, iV, iR, iG, iB ) \
    iR = iY + ( ( 1436 * ( iV - 128) ) >> 10 ); \
    iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 ); \
    iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 ); \
    CLAMP(iR) CLAMP(iG) CLAMP(iB)

static inline Void rgbToYuv( Int iR, Int iG, Int iB, Int &iY, Int &iU, Int &iV )
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

#define PEL_RGB( r, g, b ) \
    ( 0xffu << 24 ) | ( ( r & 0xff ) << 16 ) | ( ( g & 0xff ) << 8 ) | ( b & 0xff )

struct structPlaYUVerFramePelFormat
{
  const Char* name;
  Int colorSpace;
  UInt numberChannels;
  Int ratioChromaWidth;
  Int ratioChromaHeight;
  Void (*frameFromBuffer)( Pel *in, Pel*** out, UInt width, UInt height );
  Void (*bufferFromFrame)( Pel ***in, Pel* out, UInt width, UInt height );
  Void (*fillARGB32buffer)( Pel*** in, UChar* out, UInt width, UInt height );
  Int ffmpegPelFormat;
};

typedef struct structPlaYUVerFramePelFormat PlaYUVerFramePelFormat;

#define PLAYUVER_NUMBER_FORMATS 7
extern PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS];

}  // NAMESPACE

#endif // __PLAYUVERFRAMEPIXELFORMATS_H__
