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
 * \file     FrameDifference9Bits.h
 * \brief    Frame Difference module
 */

#ifndef __FRAMEDIFFERENCE9BITS_H__
#define __FRAMEDIFFERENCE9BITS_H__

#include "lib/PlaYUVerModuleIf.h"

#include <cassert>

namespace plaYUVer
{

class FrameDifference9Bits: public PlaYUVerModuleIf
{
REGISTER_CLASS_FACTORY( FrameDifference9Bits )

private:
  Int m_iMaxDiffValue;
  PlaYUVerFrame* m_pcFrameDifference9Bits;

public:
  FrameDifference9Bits();
  virtual ~FrameDifference9Bits()
  {
  }
  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
  Void destroy();

};

}  // NAMESPACE

#endif // __FRAMEDIFFERENCE9BITS_H__

