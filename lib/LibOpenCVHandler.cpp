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
 * \file     LibOpenCVHandler.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Interface with opencv lib
 */

#include <cstdio>
#include "LibOpenCVHandler.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#include "LibMemory.h"

namespace plaYUVer
{

std::vector<PlaYUVerSupportedFormat> LibOpenCVHandler::supportedReadFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  REGIST_PLAYUVER_SUPPORTED_FMT( "Portable Grayscale Map ", "pgm" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Portable Network Graphics", "png" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Joint Photographic Experts Group", "jpg" );
  REGIST_PLAYUVER_SUPPORTED_FMT( "Windows Bitmap", "bmp" );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> LibOpenCVHandler::supportedWriteFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

std::vector<PlaYUVerSupportedFormat> LibOpenCVHandler::supportedSaveFormats()
{
  INI_REGIST_PLAYUVER_SUPPORTED_FMT;
  APPEND_PLAYUVER_SUPPORTED_FMT( LibOpenCVHandler, Read );
  END_REGIST_PLAYUVER_SUPPORTED_FMT;
}

PlaYUVerFrame* LibOpenCVHandler::loadFrame( std::string filename )
{
  return NULL;
}

Bool LibOpenCVHandler::saveFrame( PlaYUVerFrame* pcFrame, std::string filename )
{
  cv::Mat* pcCvFrame = pcFrame->getCvMat();
  return cv::imwrite( filename, *pcCvFrame );
}

}  // NAMESPACE
