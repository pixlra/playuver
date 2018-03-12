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
 * \file     InterFrameVariance.cpp
 * \brief    Variance across several frames
 */

#include "InterFrameVariance.h"

#include "lib/LibMemory.h"

InterFrameVariance::InterFrameVariance()
{
  /* Module Definition */
  m_iModuleAPI = CLP_MODULE_API_2;  // Use API version 2 (recommended).
  // See this example for details on the functions prototype
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;  // Apply module to the frames or to
                                                // the whole sequence.
  m_pchModuleCategory = "Measurements";         // Category (sub-menu)
  m_pchModuleName = "InterFrameVariance";       // Name
  m_pchModuleLongName = "Variance across frames";
  m_pchModuleTooltip = "Measure the variance across several frames";
  m_uiNumberOfFrames = 2;  // Number of Frames required (This module
                           // allows a variable number of inputs)
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NEW_WINDOW | CLP_MODULES_VARIABLE_NUM_FRAMES;
  // Several requirements should be "or" between each others.
  m_pcFrameVariance = NULL;
}

bool InterFrameVariance::create( std::vector<CalypFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( unsigned int i = 1; i < apcFrameList.size(); i++ )
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], CalypFrame::MATCH_COLOR_SPACE |
                                                            CalypFrame::MATCH_RESOLUTION |
                                                            CalypFrame::MATCH_BITS ) )
      return false;

  m_pcFrameVariance =
      new CalypFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), CLP_GRAY, 8 );
  getMem2D( &m_pVariance, apcFrameList[0]->getHeight(), apcFrameList[0]->getWidth() );

  return true;
}

CalypFrame* InterFrameVariance::process( std::vector<CalypFrame*> apcFrameList )
{
  int numFrames = apcFrameList.size();

  ClpPel** pInput = new ClpPel*[numFrames];
  ClpPel* pOutputPelYUV = m_pcFrameVariance->getPelBufferYUV()[0][0];

  for( int i = 0; i < numFrames; i++ )
  {
    pInput[i] = apcFrameList[i]->getPelBufferYUV()[0][0];
  }

  double maxVariance = 0;
  for( unsigned int y = 0; y < m_pcFrameVariance->getHeight(); y++ )
    for( unsigned int x = 0; x < m_pcFrameVariance->getWidth(); x++ )
    {
      int sum = 0;
      int v = 0;
      m_pVariance[y][x] = 0;

      for( int i = 0; i < numFrames; i++ )
      {
        v = *pInput[i]++;
        sum += v;
        m_pVariance[y][x] += v * v;
      }
      // Final result of the variance for one block
      m_pVariance[y][x] -= double( sum * sum ) / double( numFrames );
      m_pVariance[y][x] /= double( numFrames );
      if( m_pVariance[y][x] > maxVariance )
        maxVariance = m_pVariance[y][x];
    }

  for( unsigned int y = 0; y < m_pcFrameVariance->getHeight(); y++ )
    for( unsigned int x = 0; x < m_pcFrameVariance->getWidth(); x++ )
    {
      *pOutputPelYUV++ = m_pVariance[y][x] * 255 / maxVariance;
    }
  delete[] pInput;
  return m_pcFrameVariance;
}

void InterFrameVariance::destroy()
{
  if( m_pVariance )
    freeMem2D( m_pVariance );
  m_pVariance = NULL;
  if( m_pcFrameVariance )
    delete m_pcFrameVariance;
  m_pcFrameVariance = NULL;
}
