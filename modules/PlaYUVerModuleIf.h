/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerModuleIf.h
 * \brief    PlaYUVer modules interface
 */

#ifndef __PLAYUVERMODULESIF_H__
#define __PLAYUVERMODULESIF_H__

#include "lib/PlaYUVerDefs.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"

namespace plaYUVer
{

#define REGISTER_CLASS_FACTORY(X) \
public: \
  static PlaYUVerModuleIf* Create() { return new X(); } \
  void Delete() { delete this; }

enum // Module type
{
  FRAME_PROCESSING_MODULE,
  FRAME_MEASUREMENT_MODULE,
};

#define APPLY_WHILE_PLAYING true
#define MAX_NUMBER_FRAMES 3

enum // Number of frames
{
  MODULE_REQUIRES_ONE_FRAME = 1,
  MODULE_REQUIRES_TWO_FRAMES = 2,
  MODULE_REQUIRES_THREE_FRAMES = 3,
};

enum // Requirements
{
  MODULE_REQUIRES_NOTHING = 0,
  MODULE_REQUIRES_NEW_WINDOW = 2,
  MODULE_REQUIRES_SIDEBAR = 4,
  MODULES_REQUIREMENTS_TOTAL,
};

typedef struct
{
  Int m_iModuleType;
  const Char* m_pchModuleCategory;
  const Char* m_pchModuleName;
  const Char* m_pchModuleTooltip;
  UInt m_uiNumberOfFrames;
  UInt m_uiModuleRequirements;
  Bool m_bApplyWhilePlaying;
} PlaYUVerModuleDefinition;

class PlaYUVerModuleIf
{
public:

  Int m_iModuleType;
  const Char* m_pchModuleCategory;
  const Char* m_pchModuleName;
  const Char* m_pchModuleTooltip;
  UInt m_uiNumberOfFrames;
  UInt m_uiModuleRequirements;
  Bool m_bApplyWhilePlaying;

  PlaYUVerModuleIf() {}
  virtual ~PlaYUVerModuleIf() {}

  virtual void Delete() = 0;

  virtual Void create() {}
  virtual Void create( PlaYUVerFrame* ) {}

  virtual PlaYUVerFrame*  process( PlaYUVerFrame* ) { return NULL; }
  virtual PlaYUVerFrame*  process( PlaYUVerFrame*, PlaYUVerFrame* ) { return NULL; }
  virtual PlaYUVerFrame*  process( PlaYUVerFrame*, PlaYUVerFrame*, PlaYUVerFrame* ) { return NULL; }

  virtual Double          measure( PlaYUVerFrame* ) { return 0; }
  virtual Double          measure( PlaYUVerFrame*, PlaYUVerFrame* ) { return 0; }
  virtual Double          measure( PlaYUVerFrame*, PlaYUVerFrame*, PlaYUVerFrame* ) { return 0; }

  virtual Void destroy() = 0;

};

}  // NAMESPACE

#endif // __PLAYUVERMODULESIF_H__

