/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     LumaAverage.cpp
 * \brief    Luma frame average
 */

#include "LumaAverage.h"

LumaAverage::LumaAverage()
{
  /* Module Definition */
  m_iModuleType = FRAME_MEASUREMENT_MODULE;                      // Apply module to the frames or to the whole sequence.
                                                                 // Currently only support for frame
  m_pchModuleCategory = "Measurements";                          // Category (sub-menu)
  m_pchModuleName = "LumaAverage";                               // Name
  m_pchModuleTooltip = "Measure the average of luma component";  // Description
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;    // Number of Frames required (ONE_FRAME, TWO_FRAMES, THREE_FRAMES)
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;  // Module requirements (check PlaYUVerModulesIf.h).
                                                     // Several requirements should be "or" between each others.
}

Void LumaAverage::create( PlaYUVerFrame* frame )
{
}

Double LumaAverage::measure( PlaYUVerFrame* frame )
{
  Double average = 0;
  Pel* pPel = frame->getPelBufferYUV()[0][0];
  for( UInt y = 0; y < frame->getHeight(); y++ )
    for( UInt x = 0; x < frame->getWidth(); x++ )
    {
      average += *pPel;
      pPel++;
    }
  return average / Double( frame->getHeight() * frame->getWidth() );
}

Void LumaAverage::destroy()
{
}
