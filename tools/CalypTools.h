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
 * \file     CalypTools.h
 * \brief    Main definition of the CalypTools app
 *
 * @defgroup CalypTools Calyp Tools
 * @{
 *
 * CalypTools is a command line interface
 * for the modules and quality metrics
 * included in Calyp
 *
 * @}
 */

#ifndef __CALYPTOOLS_H__
#define __CALYPTOOLS_H__

/**
 * @defgroup CalypTools Calyp Tools
 * @{
 * CalypTools is a command line interface
 * for the modules and quality metrics
 * included in Calyp
 * @}
 *
 */

#include "lib/CalypFrame.h"

#include "CalypToolsCmdParser.h"

class CalypFrame;
class CalypStream;
class CalypModuleIf;

#define MAX_NUMBER_INPUTS 10

class CalypTools : public CalypToolsCmdParser
{
public:
  CalypTools();
  ~CalypTools();

  int Open( int argc, char* argv[] );
  int Process();
  int Close();

private:
  bool m_bVerbose;

  unsigned int m_uiOperation;
  enum TOOLS_OPERATIONS_LIST
  {
    INVALID_OPERATION,
    SAVE_OPERATION,
    RATE_REDUCTION_OPERATION,
    QUALITY_OPERATION,
    MODULE_OPERATION,
  };

  unsigned int m_uiNumberOfFrames;
  unsigned int m_uiNumberOfComponents;
  std::vector<CalypStream*> m_apcInputStreams;
  std::vector<CalypStream*> m_apcOutputStreams;

  int openInputs();

  typedef int ( CalypTools::*FpProcess )();
  FpProcess m_fpProcess;

  long long int m_iFrameNum;
  std::vector<ClpString> m_pcOutputFileNames;
  int SaveOperation();

  int RateReductionOperation();

  int m_uiQualityMetric;
  int QualityOperation();

  CalypModuleIf* m_pcCurrModuleIf;
  CalypFrame* applyFrameModule();
  int ModuleOperation();
};

#endif  // __CALYPTOOLS_H__
