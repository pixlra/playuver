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
 * \file     LumaAverage.cpp
 * \brief    Luma frame average
 */

#include "LumaAverage.h"

LumaAverage::LumaAverage()
{
  /* Module Definition */
  m_iModuleType = CLP_FRAME_MEASUREMENT_MODULE;                  // Apply module to the frames or to
                                                                 // the whole sequence.
                                                                 // Currently only support for frame
  m_pchModuleCategory = "Measurements";                          // Category (sub-menu)
  m_pchModuleName = "LumaAverage";                               // Name
  m_pchModuleLongName = "Average";                               // Long name
  m_pchModuleTooltip = "Measure the average of luma component";  // Description
  m_uiNumberOfFrames = 1;                                        // Number of Frames required
                                                                 // (ONE_FRAME, TWO_FRAMES,
                                                                 // THREE_FRAMES)
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NOTHING;          // Module requirements
                                                                 // (check
                                                                 // CalypModulesIf.h).
  // Several requirements should be "or" between each others.
}

void LumaAverage::create( CalypFrame* frame ) {}

double LumaAverage::measure( CalypFrame* frame )
{
  double average = 0;
  ClpPel* pPel = frame->getPelBufferYUV()[0][0];
  for( unsigned int y = 0; y < frame->getHeight(); y++ )
    for( unsigned int x = 0; x < frame->getWidth(); x++ )
    {
      average += *pPel;
      pPel++;
    }
  return average / double( frame->getHeight() * frame->getWidth() );
}

void LumaAverage::destroy() {}
