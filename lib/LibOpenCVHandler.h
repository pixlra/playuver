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
 * \file     LibOpenCVHandler.h
 * \ingroup  PlaYUVerLib
 * \brief    Interface with opencv lib
 */

#ifndef __LIBOPENCVHANDLER_H__
#define __LIBOPENCVHANDLER_H__

#include <inttypes.h>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "PlaYUVerDefs.h"

namespace plaYUVer
{

class PlaYUVerFrame;

class LibOpenCVHandler
{
public:

  static std::vector<std::string> supportedReadFormatsExt();
  static std::vector<std::string> supportedReadFormatsName();
  static std::vector<std::string> supportedWriteFormatsExt();
  static std::vector<std::string> supportedWriteFormatsName();
  static std::vector<std::string> supportedSaveFormatsExt();
  static std::vector<std::string> supportedSaveFormatsName();

  LibOpenCVHandler()
  {
  }

  static PlaYUVerFrame* loadFrame( std::string filename );
  static Bool saveFrame( PlaYUVerFrame* pcFrame, std::string filename );

};

}  // NAMESPACE

#endif // __LIBOPENCVHANDLER_H__
