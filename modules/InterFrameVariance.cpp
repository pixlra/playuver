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
 * \file     InterFrameVariance.cpp
 * \brief    Variance across several frames
 */

#include "InterFrameVariance.h"
#include "lib/LibMemory.h"

InterFrameVariance::InterFrameVariance()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;  // Use API version 2 (recommended).
  // See this example for details on the functions prototype
  m_iModuleType = FRAME_PROCESSING_MODULE;  // Apply module to the frames or to
                                            // the whole sequence.
  m_pchModuleCategory = "Measurements";     // Category (sub-menu)
  m_pchModuleName = "InterFrameVariance";   // Name
  m_pchModuleTooltip = "Measure the variance across several frames";
  m_uiNumberOfFrames = MODULE_REQUIRES_SEVERAL_FRAMES;  // Number of Frames required (This module
                                                        // allows a variable number of inputs)
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW;  // Module requirements
                                                        // (check
                                                        // PlaYUVerModulesIf.h).
  // Several requirements should be "or" between each others.
  m_pcFrameVariance = NULL;
}

Bool InterFrameVariance::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  m_uiNumberOfFrames = Module_NumberOfFrames( apcFrameList.size() );

  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 1; i < apcFrameList.size(); i++ )
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0],
                                       PlaYUVerFrame::MATCH_COLOR_SPACE | PlaYUVerFrame::MATCH_RESOLUTION | PlaYUVerFrame::MATCH_BITS ) )
      return false;

  m_pcFrameVariance = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY, 8 );
  getMem2D( &m_pVariance, apcFrameList[0]->getHeight(), apcFrameList[0]->getWidth() );

  return true;
}

PlaYUVerFrame* InterFrameVariance::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Int numFrames = apcFrameList.size();

  Pel** pInput = new Pel*[numFrames];
  Pel* pOutputPelYUV = m_pcFrameVariance->getPelBufferYUV()[0][0];

  for( Int i = 0; i < numFrames; i++ ) {
    pInput[i] = apcFrameList[i]->getPelBufferYUV()[0][0];
  }

  Double maxVariance = 0;
  for( UInt y = 0; y < m_pcFrameVariance->getHeight(); y++ )
    for( UInt x = 0; x < m_pcFrameVariance->getWidth(); x++ ) {
      Int sum = 0;
      Int v = 0;
      m_pVariance[y][x] = 0;

      for( Int i = 0; i < numFrames; i++ ) {
        v = *pInput[i]++;
        sum += v;
        m_pVariance[y][x] += v * v;
      }
      // Final result of the variance for one block
      m_pVariance[y][x] -= Double( sum * sum ) / Double( numFrames );
      m_pVariance[y][x] /= Double( numFrames );
      if( m_pVariance[y][x] > maxVariance )
        maxVariance = m_pVariance[y][x];
    }

  for( UInt y = 0; y < m_pcFrameVariance->getHeight(); y++ )
    for( UInt x = 0; x < m_pcFrameVariance->getWidth(); x++ ) {
      *pOutputPelYUV++ = m_pVariance[y][x] * 255 / maxVariance;
    }
  delete[] pInput;
  return m_pcFrameVariance;
}

Void InterFrameVariance::destroy()
{
  if( m_pVariance )
    freeMem2D( m_pVariance );
  m_pVariance = NULL;
  if( m_pcFrameVariance )
    delete m_pcFrameVariance;
  m_pcFrameVariance = NULL;
}
