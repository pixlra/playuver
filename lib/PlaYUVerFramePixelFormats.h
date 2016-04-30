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
 * \file     PlaYUVerFramePixelFormats.h
 * \ingroup  PlaYUVerLib
 * \brief    Handling the pixel formats definition
 */

#ifndef __PLAYUVERFRAMEPIXELFORMATS_H__
#define __PLAYUVERFRAMEPIXELFORMATS_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerPixel.h"

namespace plaYUVer
{

#define CHROMA_RESAMPLING( X ) ( ( ( X + 1 ) >> 1 ) << 1 )

#define CHROMARATIO( SIZE, RATIO ) ( RATIO > 1 ? ( ( SIZE + 1 ) / RATIO ) : SIZE )

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

typedef struct PlaYUVerComponentDescriptor
{
  /**
   * Which of the 4 planes contains the component.
   */
  UInt16 plane;

  /**
   * Number of elements between 2 horizontally consecutive pixels minus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  UInt16 step_minus1;

  /**
   * Number of elements before the component of the first pixel plus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  UInt16 offset_plus1;

  /**
   * Number of least significant bits that must be shifted away
   * to get the value.
   */
  UInt16 shift;

  /**
   * Number of bits in the component minus 1.
   */
  UInt16 depth_minus1;
} PlaYUVerComponentDescriptor;

/**
 * Descriptor that unambiguously describes how the bits of a pixel are
 * stored in the up to 4 data planes of an image. It also stores the
 * subsampling factors and number of components.
 *
 * @note This is separate of the colorspace (RGB, YCbCr, YPbPr, JPEG-style YUV
 *       and all the YUV variants) AVPixFmtDescriptor just stores how values
 *       are stored not what these values represent.
 */
typedef struct PlaYUVerPixFmtDescriptor
{
  const char *name;

  Int colorSpace;

  UInt8 numberChannels;  ///< The number of components each pixel has, (1-4)

  UInt8 numberPlanes;  ///< The number of planes that should be read from the file

  /**
   * Amount to shift the luma width right to find the chroma width.
   * For YV12 this is 1 for example.
   * chroma_width = -((-luma_width) >> log2ChromaWidth)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  UInt8 log2ChromaWidth;  ///< chroma_width = -((-luma_width )>>log2ChromaWidth)

  /**
   * Amount to shift the luma height right to find the chroma height.
   * For YV12 this is 1 for example.
   * chroma_height= -((-luma_height) >> log2ChromaHeight)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  UInt8 log2ChromaHeight;

  Int ffmpegPelFormat;

  /**
   * Parameters that describe how pixels are packed.
   * If the format has 2 or 4 components, then alpha is last.
   * If the format has 1 or 2 components, then luma is 0.
   * If the format has 3 or 4 components,
   * if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
   * otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
   */
  PlaYUVerComponentDescriptor comp[4];

} PlaYUVerPixFmtDescriptor;

#define MAX_NUMBER_PLANES 3

#define PLAYUVER_NUMBER_FORMATS 7
extern PlaYUVerPixFmtDescriptor g_PlaYUVerPixFmtDescriptorsList[PLAYUVER_NUMBER_FORMATS];

}  // NAMESPACE

#endif // __PLAYUVERFRAMEPIXELFORMATS_H__
