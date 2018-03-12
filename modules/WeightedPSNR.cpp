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
 * \file     WeightedPSNR.cpp
 * \brief    Weighted PSNR quality module
 */

#include "WeightedPSNR.h"

#include <cmath>

WeightedPSNR::WeightedPSNR()
{
  /* Module Definition */
  m_iModuleAPI = CLP_MODULE_API_2;
  m_iModuleType = CLP_FRAME_MEASUREMENT_MODULE;
  m_pchModuleCategory = "Quality";
  m_pchModuleName = "WeightedPSNR";
  m_pchModuleLongName = "Weighted PSNR";
  m_pchModuleTooltip = "Measure the weighted PSNR between two images";
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_OPTIONS;
  m_uiNumberOfFrames = 3;

  m_cModuleOptions.addOptions() /**/
      ( "Component", m_uiComponent, "Select the component to compute quality [0]" );

  m_uiComponent = 0;
}

bool WeightedPSNR::create( std::vector<CalypFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( unsigned int i = 0; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], CalypFrame::MATCH_RESOLUTION ) )
      return false;
  }
  if( apcFrameList[0]->getNumberChannels() != 1 )
  {
    return false;
  }
  for( unsigned int i = 1; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[1], CalypFrame::MATCH_COLOR_SPACE | CalypFrame::MATCH_BITS ) )
      return false;
  }
  return true;
}

double measureWMSE( int component, CalypFrame* Org, CalypFrame* Rec, CalypFrame* Mask )
{
  ClpPel* pMaskPelYUV = Mask->getPelBufferYUV()[0][0];
  ClpPel* pRecPelYUV = Rec->getPelBufferYUV()[component][0];
  ClpPel* pOrgPelYUV = Org->getPelBufferYUV()[component][0];

  double aux_pel_1, aux_pel_2, aux_pel_mask;
  int diff = 0;
  double ssd = 0;
  unsigned int count = 0;

  unsigned int numberOfPixels = 0;
  if( component == CLP_LUMA )
  {
    numberOfPixels = Org->getHeight() * Org->getWidth();
  }
  else
  {
    numberOfPixels = Rec->getChromaLength();
  }

  for( unsigned int i = 0; i < numberOfPixels; i++ )
  {
    aux_pel_mask = *pMaskPelYUV++;
    aux_pel_1 = *pRecPelYUV++;
    aux_pel_2 = *pOrgPelYUV++;
    diff = aux_pel_1 - aux_pel_2;
    ssd += ( aux_pel_mask * diff * diff );
    count += aux_pel_mask;
  }
  if( ssd == 0.0 )
  {
    return 0.0;
  }
  return ssd / double( count );
}

double WeightedPSNR::measure( std::vector<CalypFrame*> apcFrameList )
{
  double dPSNR = 100;
  double dMaxValue = ( 1 << apcFrameList[1]->getBitsPel() ) - 1;
  double dMSE = measureWMSE( m_uiComponent, apcFrameList[1], apcFrameList[2], apcFrameList[0] );
  if( dMSE != 0 )
    dPSNR = 10 * log10( ( dMaxValue * dMaxValue ) / dMSE );
  return dPSNR;
}

void WeightedPSNR::destroy() {}
