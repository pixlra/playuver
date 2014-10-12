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
 * \file     PlaYUVerFrame.cpp
 * \brief    Video Frame handling
 */

#include "config.h"
#include <cstdio>
#include "LibMemory.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

Void frameFromBuffer_yuv420p( Pel *in, Pel*** out )
{

}
Void bufferFromFrame_yuv420p( Pel ***in, Pel* out )
{

}
Void fillRGBbuffer_yuv420p( Pel*** in, UChar* out )
{

}
Pixel getPixelValueYUV420p ( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos >> 1][xPos >> 1];
  pixel_value.ChromaV = Img[CHROMA_V][yPos >> 1][xPos >> 1];
  return pixel_value;
}

Pixel getPixelValueYUV444p ( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos][xPos];
  pixel_value.ChromaV = Img[CHROMA_V][yPos][xPos];
  return pixel_value;
}

Pixel getPixelValueYUV422 ( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos][xPos >> 1];
  pixel_value.ChromaV = Img[CHROMA_V][yPos][xPos >> 1];
  return pixel_value;
}

Pixel getPixelValueGray ( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = 0;
  pixel_value.ChromaV = 0;
  return pixel_value;
}

Pixel getPixelValueRGB8 ( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_RGB;
  pixel_value.ColorR = Img[COLOR_R][yPos][xPos];
  pixel_value.ColorG = Img[COLOR_G][yPos][xPos];
  pixel_value.ColorB = Img[COLOR_B][yPos][xPos];
  return pixel_value;
}

PlaYUVerFramePelFormat yuv420p =
{
  "YUV420p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  2,
  getPixelValueYUV420p,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

PlaYUVerFramePelFormat yuv444p =
{
  "YUV444p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  1,
  1,
  getPixelValueYUV444p,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

PlaYUVerFramePelFormat yuv422p =
{
  "YUV422p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  1,
  getPixelValueYUV422,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

PlaYUVerFramePelFormat yuv422 =
{
  "YUV422",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  1,
  getPixelValueYUV422,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

PlaYUVerFramePelFormat gray =
{
  "GRAY",
  PlaYUVerFrame::COLOR_GRAY,
  1,
  0,
  0,
  getPixelValueGray,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

PlaYUVerFramePelFormat rgb8 =
{
  "RGB8",
  PlaYUVerFrame::COLOR_RGB,
  3,
  1,
  1,
  getPixelValueRGB8,
  frameFromBuffer_yuv420p,
  bufferFromFrame_yuv420p,
  fillRGBbuffer_yuv420p,
};

extern PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS] =
{
  yuv420p,
  yuv444p,
  yuv422p,
  yuv422,
  gray,
  rgb8,
};

}  // NAMESPACE
