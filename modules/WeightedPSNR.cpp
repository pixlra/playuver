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
 * \file     WeightedPSNR.cpp
 * \brief    Weighted PSNR quality module
 */

#include "WeightedPSNR.h"
#include <algorithm>
#include <cmath>


WeightedPSNR::WeightedPSNR()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_MEASUREMENT_MODULE;
  m_pchModuleCategory = "Quality";
  m_pchModuleName = "WeightedPSNR";
  m_pchModuleTooltip = "Measure the weighted PSNR between two images";
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS;
  m_uiNumberOfFrames = MODULE_REQUIRES_THREE_FRAMES;

  m_cModuleOptions.addOptions()/**/
  ( "Component", m_uiComponent, "Select the component to compute quality [0]" );

  m_uiComponent = 0;

}

Bool WeightedPSNR::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 0; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], PlaYUVerFrame::MATCH_RESOLUTION ) )
      return false;
  }
  if( apcFrameList[0]->getNumberChannels() != 1 )
  {
    return false;
  }
  for( UInt i = 1; i < apcFrameList.size(); i++ )
  {
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[1], PlaYUVerFrame::MATCH_COLOR_SPACE | PlaYUVerFrame::MATCH_BITS ) )
      return false;
  }
  return true;
}

Double measureWMSE( Int component, PlaYUVerFrame* Org, PlaYUVerFrame* Rec, PlaYUVerFrame* Mask )
{
  Pel* pMaskPelYUV = Mask->getPelBufferYUV()[0][0];
  Pel* pRecPelYUV = Rec->getPelBufferYUV()[component][0];
  Pel* pOrgPelYUV = Org->getPelBufferYUV()[component][0];

  Double aux_pel_1, aux_pel_2, aux_pel_mask;
  Int diff = 0;
  Double ssd = 0;
  UInt count = 0;

  UInt numberOfPixels = 0;
  if( component == LUMA )
  {
    numberOfPixels = Org->getHeight() * Org->getWidth();
  }
  else
  {
    numberOfPixels = Rec->getChromaLength();
  }

  for( UInt i = 0; i < numberOfPixels; i++ )
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
  return ssd / Double( count );
}


Double WeightedPSNR::measure( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Double dPSNR = 100;
  Double dMaxValue = ( 1 << apcFrameList[1]->getBitsPel() ) - 1;
  Double dMSE = measureWMSE( m_uiComponent, apcFrameList[1], apcFrameList[2], apcFrameList[0] );
  if( dMSE != 0 )
    dPSNR = 10 * log10( ( dMaxValue * dMaxValue ) / dMSE );
  return dPSNR;
}

Void WeightedPSNR::destroy()
{

}



