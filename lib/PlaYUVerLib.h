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
 * \file     PlaYUVerLib.h
 * \ingroup  PlaYUVerLib
 * \brief    Inlcude all modules of PlaYUVer Lib
 */

#ifndef __PLAYUVERLIB_H_
#define __PLAYUVERLIB_H_

/**
 * @defgroup PlaYUVerLib PlaYUVer Lib
 * @{
 *
 * PlaYUVerLib is an independent module that provide the low level interface
 * for dealing with streams and frames.
 *
 * Two main interfaces are defining PlaYUVerFrame and PlaYUVerStream.
 * Moreover handler for external libs, as FFmpeg and OpenCv are provided
 * which enables PlaYUVer to boost its functionality using external libs.
 *
 * This guaratees that the top-level applications does not need to worry
 * about dealig with different stream formats as well as different pixel formats
 *
 * @defgroup PlaYUVerLib_Stream PlaYUVerStream
 * @{
 *  Describe PlaYUVerStream
 * @}
 *
 * @defgroup PlaYUVerLib_Frame PlaYUVerFrame
 * @{
 *  Describe PlaYUVerFrame
 * @}
 *
 * @}
 *
 */

#include "PlaYUVerFrame.h"
#include "PlaYUVerStream.h"

#endif // __PLAYUVERLIB_H_
