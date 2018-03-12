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
 * \file     MeasureOpticalFlowDualTVL1.h
 * \brief    Absolute Frame Difference module
 */

#ifndef __MeasureOpticalFlowDualTVL1_H__
#define __MeasureOpticalFlowDualTVL1_H__

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

// CalypLib
#include "lib/CalypModuleIf.h"

class MeasureOpticalFlowDualTVL1 : public CalypModuleIf
{
  REGISTER_CLASS_FACTORY( MeasureOpticalFlowDualTVL1 )

private:
  bool m_bShowReconstruction;
  int m_iStep;
  cv::Ptr<cv::DenseOpticalFlow> m_cTvl1;
  cv::Mat_<cv::Point2f> m_cvFlow;
  CalypFrame* m_pcFramePrev;
  CalypFrame* m_pcFrameAfter;
  CalypFrame* m_pcOutputFrame;

  void drawFlow();
  void compensateFlow();

public:
  MeasureOpticalFlowDualTVL1();
  virtual ~MeasureOpticalFlowDualTVL1() {}
  bool create( std::vector<CalypFrame*> apcFrameList );
  CalypFrame* process( std::vector<CalypFrame*> apcFrameList );
  void destroy();
};

#endif  // __MeasureOpticalFlowDualTVL1_H__
