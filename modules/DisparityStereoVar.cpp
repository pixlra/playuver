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
 * \file     DisparityStereoVar.cpp
 * \brief    Measure the disparity between two images using the Stereo Var method (OpenCV)
 */

#include <cstdio>

#include "DisparityStereoVar.h"


DisparityStereoVar::DisparityStereoVar()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Disparity";
  m_pchModuleName = "StereoVar";
  m_pchModuleTooltip = "Measure the disparity between two images using the Stereo Var method (OpenCV)";
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;
  m_uiModuleRequirements = MODULE_REQUIRES_SKIP_WHILE_PLAY | MODULE_REQUIRES_NEW_WINDOW;

  m_pcDisparityFrame = NULL;
  m_cStereoVar.levels = 3;                                 // ignored with USE_AUTO_PARAMS
  m_cStereoVar.pyrScale = 0.5;                             // ignored with USE_AUTO_PARAMS
  m_cStereoVar.nIt = 25;

  m_cStereoVar.maxDisp = 0;
  m_cStereoVar.poly_n = 3;
  m_cStereoVar.poly_sigma = 0.0;
  m_cStereoVar.fi = 15.0f;
  m_cStereoVar.lambda = 0.03f;
  m_cStereoVar.penalization = m_cStereoVar.PENALIZATION_TICHONOV;            // ignored with USE_AUTO_PARAMS
  m_cStereoVar.cycle = m_cStereoVar.CYCLE_V;                                 // ignored with USE_AUTO_PARAMS
  m_cStereoVar.flags = m_cStereoVar.USE_SMART_ID | m_cStereoVar.USE_AUTO_PARAMS | m_cStereoVar.USE_INITIAL_DISPARITY | m_cStereoVar.USE_MEDIAN_FILTERING;
}

Bool DisparityStereoVar::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_
  m_pcDisparityFrame = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY );
  m_cStereoVar.minDisp = -( ( ( apcFrameList[0]->getWidth() / 8 ) + 15 ) & -16 );
  return true;
}
PlaYUVerFrame* DisparityStereoVar::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  PlaYUVerFrame* InputLeft = apcFrameList[0];
  PlaYUVerFrame* InputRight = apcFrameList[1];

  cv::Mat* leftImage = InputLeft->getCvMat();
  cv::Mat* rightImage = InputRight->getCvMat();
  cv::Mat disparityImage, disparityImage8;

  m_cStereoVar( *leftImage, *rightImage, disparityImage );
  disparityImage.convertTo( disparityImage8, CV_8U );

  m_pcDisparityFrame->fromCvMat( &disparityImage8 );
  return m_pcDisparityFrame;
}

Void DisparityStereoVar::destroy()
{
  if( m_pcDisparityFrame )
    delete m_pcDisparityFrame;
  m_pcDisparityFrame = NULL;
}



