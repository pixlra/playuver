/*    This file is a part of PlaYUVer project
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
 * \file     MeasureOpticalFlowDualTVL1.cpp
 * \brief    Absolute Frame Difference module
 */

#include "MeasureOpticalFlowDualTVL1.h"

using cv::Mat;
using cv::Mat_;
using cv::Point;
using cv::Point2f;
using cv::Ptr;
using cv::Scalar;

MeasureOpticalFlowDualTVL1::MeasureOpticalFlowDualTVL1()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Measurements";
  m_pchModuleName = "MeasureOpticalFlowDualTVL1";
	m_pchModuleLongName = "Optical flow based on DualTVL1";
  m_pchModuleTooltip = "Measure optical flow";
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;
  m_uiModuleRequirements = MODULE_REQUIRES_SKIP_WHILE_PLAY | MODULE_REQUIRES_NEW_WINDOW | MODULE_REQUIRES_OPTIONS;

  m_cModuleOptions.addOptions() /**/
      ( "Show reconstruction", m_bShowReconstruction, "Show reconstructed frame instead of MVs [false]" );

  m_bShowReconstruction = false;
  m_pcOutputFrame = NULL;
}

Bool MeasureOpticalFlowDualTVL1::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 1; i < apcFrameList.size(); i++ )
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], PlaYUVerFrame::MATCH_COLOR_SPACE |
                                                            PlaYUVerFrame::MATCH_RESOLUTION |
                                                            PlaYUVerFrame::MATCH_BITS ) )
      return false;

  m_iStep = 16;
  m_cTvl1 = cv::createOptFlow_DualTVL1();
  m_pcOutputFrame = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY );

  return true;
}

static inline bool isFlowCorrect( Point2f u )
{
  return !cvIsNaN( u.x ) && !cvIsNaN( u.y ) && fabs( u.x ) < 1e9 && fabs( u.y ) < 1e9;
}

Void MeasureOpticalFlowDualTVL1::drawFlow()
{
  Mat cvMatAfter;
  m_pcFrameAfter->toMat( cvMatAfter, true );
  Scalar vectorColor( 255, 0, 0, 0 );
  for( int y = m_iStep / 2; y < m_cvFlow.rows; y += m_iStep )
  {
    for( int x = m_iStep / 2; x < m_cvFlow.cols; x += m_iStep )
    {
      Point2f u( 0, 0 );
      Double count = 0;
      for( int i = ( -m_iStep / 2 ); i < ( m_iStep / 2 ); i++ )
      {
        for( int j = ( -m_iStep / 2 ); j < ( m_iStep / 2 ); j++ )
        {
          if( isFlowCorrect( u ) )
          {
            u = u + m_cvFlow( y, x );
            count++;
          }
        }
      }
      if( count > 0 )
      {
        Point p = Point( x, y );
        Point ui( u.x / count, u.y / count );
        arrowedLine( cvMatAfter, p, p + ui, vectorColor );
      }
    }
  }
  m_pcOutputFrame->fromMat( cvMatAfter );
}

Void MeasureOpticalFlowDualTVL1::compensateFlow()
{
  for( UInt c = 0; c < m_pcOutputFrame->getNumberChannels(); c++ )
  {
    Pel** pPelPrev = m_pcFramePrev->getPelBufferYUV()[c];
    Pel* pPelOut = m_pcOutputFrame->getPelBufferYUV()[c][0];

    for( UInt y = 0; y < m_pcOutputFrame->getHeight( c ); y++ )
    {
      for( UInt x = 0; x < m_pcOutputFrame->getWidth( c ); x++ )
      {
        Point2f u = m_cvFlow( y, x );
        Point p( x + u.x, y + u.y );
        *pPelOut = pPelPrev[p.y][p.x];
        pPelOut++;
      }
    }
  }
}

PlaYUVerFrame* MeasureOpticalFlowDualTVL1::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  m_pcFrameAfter = apcFrameList[0];
  m_pcFramePrev = apcFrameList[1];

  Mat cvMatPrev, cvMatAfter;
  if( !m_pcFramePrev->toMat( cvMatPrev, true ) || !m_pcFrameAfter->toMat( cvMatAfter, true ) )
  {
    return m_pcOutputFrame;
  }

  m_cTvl1->calc( cvMatPrev, cvMatAfter, m_cvFlow );
  if( m_bShowReconstruction )
    compensateFlow();
  else
    drawFlow();

  return m_pcOutputFrame;
}

Void MeasureOpticalFlowDualTVL1::destroy()
{
  if( m_pcOutputFrame )
    delete m_pcOutputFrame;
  m_pcOutputFrame = NULL;
}
