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
 * \file     OpticalFlow.h
 * \brief    Modules to measure optical flow
 */

#ifndef __OPTICALFLOWDUALTVL1_H__
#define __OPTICALFLOWDUALTVL1_H__

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

// CalypLib
#include "lib/CalypModuleIf.h"

class OpticalFlowModule : public CalypModuleIf
{
protected:
  bool m_bShowReconstruction;
  int m_iStep;
  cv::Ptr<cv::DenseOpticalFlow> m_cOpticalFlow;
  cv::Mat_<cv::Point2f> m_cvFlow;
  CalypFrame* m_pcFramePrev;
  CalypFrame* m_pcFrameAfter;
  CalypFrame* m_pcOutputFrame;
  void drawFlow();
  void compensateFlow();
  bool commonCreate( std::vector<CalypFrame*> apcFrameList );

public:
  OpticalFlowModule();
  virtual ~OpticalFlowModule() {}
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList );
  void destroy();
};

class OpticalFlowDualTVL1 : public OpticalFlowModule
{
  REGISTER_CLASS_FACTORY( OpticalFlowDualTVL1 )
public:
  OpticalFlowDualTVL1();
  virtual ~OpticalFlowDualTVL1() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
};

class OpticalFlowSparseToDense : public OpticalFlowModule
{
  REGISTER_CLASS_FACTORY( OpticalFlowSparseToDense )
public:
  OpticalFlowSparseToDense();
  virtual ~OpticalFlowSparseToDense() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
};

class OpticalFlowFarneback : public OpticalFlowModule
{
  REGISTER_CLASS_FACTORY( OpticalFlowFarneback )
public:
  OpticalFlowFarneback();
  virtual ~OpticalFlowFarneback() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
};

class OpticalDeepFlow : public OpticalFlowModule
{
  REGISTER_CLASS_FACTORY( OpticalDeepFlow )
public:
  OpticalDeepFlow();
  virtual ~OpticalDeepFlow() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
};

#endif  // __OPTICALFLOW_H__
