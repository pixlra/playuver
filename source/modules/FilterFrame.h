/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
 * \file     FilterFrame.h
 * \brief    Filter frame module
 */

#ifndef __FILTERFRAME_H__
#define __FILTERFRAME_H__

#include "config.h"

#include <iostream>
#include <cstdio>
#include "TypeDef.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerModuleIf.h"

#include <cassert>

namespace plaYUVer
{

const Char ModuleType[] = "Filter";
const Char ModuleName[] = "Y_Filter";
const Char ModuleTooltip[] = "Filter Y matrix of YUV frame";

class FilterFrame: public PlaYUVerModuleIf
{
private:
  PlaYUVerFrame* m_pcFilteredFrame;

public:
  FilterFrame()
  {
    REGISTER_MODULE_NAME(ModuleType, ModuleName, ModuleTooltip )
    m_pcFilteredFrame = NULL;
  }
  virtual ~FilterFrame()
  {
  }

  Void create( PlaYUVerFrame* InputFrame );
  PlaYUVerFrame* process( PlaYUVerFrame* InputFrame );
  Void destroy();
};

}  // NAMESPACE

#endif // __FILTERFRAME_H__

