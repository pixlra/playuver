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
 * \file     FilterComponent.h
 * \brief    Filter component frame module
 */

#ifndef __FILTERCOMPONENT_H__
#define __FILTERCOMPONENT_H__

#include "PlaYUVerModuleIf.h"

namespace plaYUVer
{

class FilterComponentModule: public PlaYUVerModuleIf
{
private:
  PlaYUVerFrame* m_pcFilteredFrame;
public:
  FilterComponentModule();
  virtual ~FilterComponentModule()
  {
  }
  Void create( PlaYUVerFrame* ) = 0;
  PlaYUVerFrame* process( PlaYUVerFrame* ) = 0;
  Void destroy();

  Void createFilter( UInt uiWidth, UInt uiHeight, UInt bitsPixel );
  PlaYUVerFrame* filterComponent( PlaYUVerFrame* InputFrame,  Int Component );
};

class FilterComponentLuma: public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentLuma )
public:
  FilterComponentLuma();
  virtual ~FilterComponentLuma()
  {
  }
  Void create( PlaYUVerFrame* InputFrame );
  PlaYUVerFrame* process( PlaYUVerFrame* InputFrame );
};

class FilterComponentChromaU: public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentChromaU )
public:
  FilterComponentChromaU();
  virtual ~FilterComponentChromaU()
  {
  }
  Void create( PlaYUVerFrame* InputFrame );
  PlaYUVerFrame* process( PlaYUVerFrame* InputFrame );
};

class FilterComponentChromaV: public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentChromaV )
public:
  FilterComponentChromaV();
  virtual ~FilterComponentChromaV()
  {
  }
  Void create( PlaYUVerFrame* InputFrame );
  PlaYUVerFrame* process( PlaYUVerFrame* InputFrame );
};

}  // NAMESPACE

#endif // __FILTERFRAME_H__

