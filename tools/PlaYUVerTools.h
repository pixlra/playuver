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
 * \file     PlaYUVerTools.h
 * \brief    Main definition of the PlaYUVerTools app
 */

#ifndef __PLAYUVERTOOLS_H__
#define __PLAYUVERTOOLS_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"
#include "PlaYUVerToolsCmdParser.h"

class PlaYUVerModuleIf;

#define MAX_NUMBER_INPUTS 10

class PlaYUVerTools: public PlaYUVerToolsCmdParser
{
public:
  PlaYUVerTools();
  ~PlaYUVerTools();

  Int Open( Int argc, Char *argv[] );
  Int Process();
  Int Close();

private:
  Bool m_bVerbose;

  UInt m_uiOperation;
  enum TOOLS_OPERATIONS_LIST
  {
    INVALID_OPERATION,
    SAVE_OPERATION,
    QUALITY_OPERATION,
    MODULE_OPERATION,
  };

  UInt m_uiNumberOfFrames;
  UInt m_uiNumberOfComponents;
  std::vector<PlaYUVerStream*> m_apcInputStreams;
  std::vector<PlaYUVerStream*> m_apcOutputStreams;

  Int openInputs();

  typedef Int (PlaYUVerTools::*FpProcess)();
  FpProcess m_fpProcess;

  Int64 m_iFrameNum;
  std::vector<String> m_pcOutputFileNames;
  Int SaveOperation();

  Int m_uiQualityMetric;
  Int QualityOperation();

  PlaYUVerModuleIf* m_pcCurrModuleIf;
  PlaYUVerFrame* applyFrameModule();
  Int ModuleOperation();
};

#endif // __PLAYUVERTOOLS_H__
