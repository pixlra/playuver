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
 * \file     FrameBinarization_APIv1.h
 * \brief    Binarize frame module (example of APIv1)
 */

#ifndef __FRAMEBINARIZATION_H__
#define __FRAMEBINARIZATION_H__

#include "PlaYUVerModuleIf.h"

class FrameBinarization: public PlaYUVerModuleIf
{
REGISTER_CLASS_FACTORY( FrameBinarization )

private:
  PlaYUVerFrame* m_pcBinFrame;
  UInt m_uiThreshold;
  public:
  FrameBinarization();
  virtual ~FrameBinarization()
  {
  }
  Void create( PlaYUVerFrame* );
  PlaYUVerFrame* process( PlaYUVerFrame* );
  Void destroy();

};

#endif // __FRAMEBINARIZATION_H__

