/*    This file is a part of Calyp project
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
 * \file     FilterComponent.h
 * \brief    Filter component frame module
 */

#ifndef __FILTERCOMPONENT_H__
#define __FILTERCOMPONENT_H__

// CalypLib
#include "lib/CalypModuleIf.h"

class FilterComponentModule : public CalypModuleIf
{
private:
  CalypFrame* m_pcFilteredFrame;

public:
  FilterComponentModule();
  virtual ~FilterComponentModule() {}
  bool create( std::vector<CalypFrame*> apcFrameList ) = 0;
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList ) = 0;
  void destroy();

  bool createFilter( unsigned int uiWidth, unsigned int uiHeight, unsigned int bitsPixel );
  CalypFrame* filterComponent( CalypFrame* InputFrame, int Component );
};

class FilterComponentLuma : public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentLuma )
public:
  FilterComponentLuma();
  virtual ~FilterComponentLuma() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList );
};

class FilterComponentChromaU : public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentChromaU )
public:
  FilterComponentChromaU();
  virtual ~FilterComponentChromaU() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList );
};

class FilterComponentChromaV : public FilterComponentModule
{
  REGISTER_CLASS_FACTORY( FilterComponentChromaV )
public:
  FilterComponentChromaV();
  virtual ~FilterComponentChromaV() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList );
};

#endif  // __FILTERFRAME_H__
