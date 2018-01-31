/*    This file is a part of PlaYUVer project
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
 * \file     FrameRotate.h
 * \brief    Module to aapply a mask to an image
 */

#ifndef __FRAMEROTATE_H__
#define __FRAMEROTATE_H__

// PlaYUVerLib
#include "lib/PlaYUVerModuleIf.h"

class FrameRotate : public PlaYUVerModuleIf
{
  REGISTER_CLASS_FACTORY( FrameRotate )

private:
  Int m_iAngle;
  PlaYUVerFrame* m_pcFrameProcessed;

public:
  FrameRotate();
  virtual ~FrameRotate() {}
  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
  Void destroy();
};

#endif  // __FRAMEROTATE_H__
