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

std::vector<std::string> LibOpenCVHandler::supportedReadFormatsExt()
{
  std::vector<std::string> formatsExt;
  formatsExt.push_back( "pgm" );
  formatsExt.push_back( "png" );
  formatsExt.push_back( "bmp" );
  formatsExt.push_back( "jpg" );
  return formatsExt;
}

std::vector<std::string> LibOpenCVHandler::supportedReadFormatsName()
{
  std::vector<std::string> formatsName;
  formatsName.push_back( "Portable Grayscale Map" );
  formatsName.push_back( "Portable Network Graphics" );
  formatsName.push_back( "Windows Bitmap" );
  formatsName.push_back( "Joint Photographic Experts Group" );
  return formatsName;
}

std::vector<std::string> LibOpenCVHandler::supportedWriteFormatsExt()
{
  std::vector<std::string> formatsExt;
  return formatsExt;
}

std::vector<std::string> LibOpenCVHandler::supportedWriteFormatsName()
{
  std::vector<std::string> formatsName;
  return formatsName;
}

std::vector<std::string> LibOpenCVHandler::supportedSaveFormatsExt()
{
  return supportedReadFormatsExt();
}

std::vector<std::string> LibOpenCVHandler::supportedSaveFormatsName()
{
  return supportedReadFormatsName();
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
