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
 * \file     PixelFormats.h
 * \brief    Handling the pixel formats definition
 */

#ifndef __PIXELFORMATS_H__
#define __PIXELFORMATS_H__

#include "CalypFrame.h"

#include <map>

#define CHROMA_RESAMPLING( X ) ( ( ( X + 1 ) >> 1 ) << 1 )

#define CHROMARATIO( SIZE, RATIO ) ( RATIO > 1 ? ( ( SIZE + 1 ) / RATIO ) : SIZE )

typedef struct CalypComponentDescriptor
{
  /**
   * Which of the 4 planes contains the component.
   */
  unsigned short plane;

  /**
   * Number of elements between 2 horizontally consecutive pixels minus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  unsigned short step_minus1;

  /**
   * Number of elements before the component of the first pixel plus 1.
   * Elements are bits for bitstream formats, bytes otherwise.
   */
  unsigned short offset_plus1;

} CalypComponentDescriptor;

/**
 * Descriptor that unambiguously describes how the bits of a pixel are
 * stored in the up to 4 data planes of an image. It also stores the
 * subsampling factors and number of components.
 *
 * @note This is separate of the colorspace (RGB, YCbCr, YPbPr, JPEG-style YUV
 *       and all the YUV variants) AVPixFmtDescriptor just stores how values
 *       are stored not what these values represent.
 */
typedef struct CalypPixelFormatDescriptor
{
  const char* name;

  int colorSpace;

  unsigned char numberChannels;  ///< The number of components each pixel has, (1-4)

  unsigned char numberPlanes;  ///< The number of planes that should be read from the file

  /**
   * Amount to shift the luma width right to find the chroma width.
   * For YV12 this is 1 for example.
   * chroma_width = -((-luma_width) >> log2ChromaWidth)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  unsigned char log2ChromaWidth;  ///< chroma_width = -((-luma_width )>>log2ChromaWidth)

  /**
   * Amount to shift the luma height right to find the chroma height.
   * For YV12 this is 1 for example.
   * chroma_height= -((-luma_height) >> log2ChromaHeight)
   * The note above is needed to ensure rounding up.
   * This value only refers to the chroma components.
   */
  unsigned char log2ChromaHeight;

  int ffmpegPelFormat;

  /**
   * Parameters that describe how pixels are packed.
   * If the format has 2 or 4 components, then alpha is last.
   * If the format has 1 or 2 components, then luma is 0.
   * If the format has 3 or 4 components,
   * if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
   * otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
   */
  CalypComponentDescriptor comp[4];

} CalypPixelFormatDescriptor;

#define MAX_NUMBER_PLANES 3

extern const std::map<int, CalypPixelFormatDescriptor> g_CalypPixFmtDescriptorsMap;

#endif  // __PIXELFORMATS_H__
