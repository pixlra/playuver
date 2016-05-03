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
 * \file     OpticalFlowDualTVL1.cpp
 * \brief    Absolute Frame Difference module
 */

#include "OpticalFlowDualTVL1.h"
#include <cstdlib>

using cv::Scalar;
using cv::Mat;
using cv::Mat_;
using cv::Ptr;
using cv::Point;
using cv::Point2f;



OpticalFlowDualTVL1::OpticalFlowDualTVL1()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;                              // Use API version 2 (recommended).
  // See this example for details on the functions prototype
  m_iModuleType = FRAME_PROCESSING_MODULE;                  // Apply module to the frames or to the whole sequence.
  m_pchModuleCategory = "Measurements";                     // Category (sub-menu)
  m_pchModuleName = "OpticalFlowDualTVL1";                  // Name
  m_pchModuleTooltip = "Measure optical flow";              // Description
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;          // Number of Frames required (ONE_FRAME, TWO_FRAMES, THREE_FRAMES)
  m_uiModuleRequirements = MODULE_REQUIRES_SKIP_WHILE_PLAY | MODULE_REQUIRES_NEW_WINDOW;
  m_pcOutputFrame = NULL;
}

Bool OpticalFlowDualTVL1::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 1; i < apcFrameList.size(); i++ )
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0], PlaYUVerFrame::MATCH_COLOR_SPACE | PlaYUVerFrame::MATCH_RESOLUTION | PlaYUVerFrame::MATCH_BITS ) )
      return false;

  m_iStep = 16;
  m_cTvl1 = cv::createOptFlow_DualTVL1();
  m_pcOutputFrame = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY );

  return true;
}

inline bool isFlowCorrect( Point2f u )
{
  return !cvIsNaN( u.x ) && !cvIsNaN( u.y ) && fabs( u.x ) < 1e9 && fabs( u.y ) < 1e9;
}

PlaYUVerFrame* OpticalFlowDualTVL1::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Mat cvMatPrev, cvMatAfter;
  if( !apcFrameList[0]->toMat( cvMatPrev, true ) || !apcFrameList[1]->toMat( cvMatAfter, true ) )
  {
    return m_pcOutputFrame;
  }

  Mat_<Point2f> flow;
  m_cTvl1->calc( cvMatPrev, cvMatAfter, flow );


  // determine motion range:
  Double maxrad = 1;

  for( int y = 0; y < flow.rows; y++ )
  {
    for( int x = 0; x < flow.cols; x++ )
    {
      Point2f u = flow( y, x );
      if( !isFlowCorrect( u ) )
        continue;
      maxrad = cv::max( maxrad, sqrt( u.x * u.x + u.y * u.y ) );
    }
  }

  Scalar vectorColor( 255,0,0,0 );
  for( int y = 0; y < flow.rows; y+=m_iStep )
  {
    for( int x = 0; x < flow.cols; x+=m_iStep )
    {
      Point p = Point( x,y );
      Point2f u = flow( y, x );
      Point ui( u.x, u.y );

      if( isFlowCorrect( u ) )
        arrowedLine( cvMatAfter, p, p+ui, vectorColor );
    }
  }

  m_pcOutputFrame->fromMat( cvMatAfter );
  return m_pcOutputFrame;
}

Void OpticalFlowDualTVL1::destroy()
{
  if( m_pcOutputFrame )
    delete m_pcOutputFrame;
  m_pcOutputFrame = NULL;
}



