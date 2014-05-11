/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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

#include "config.h"

#include <iostream>
#include <cstdio>

#include "TypeDef.h"
#include "PlaYUVerFrame.h"

class QImage;
class QAction;

namespace plaYUVer
{

enum __PlaYUVerModuleTypes
{
  FRAME_LEVEL_MODULE,
  VIDEO_LEVEL_MODULE,
};

enum __PlaYUVerModuleRequirements
{
  MODULE_REQUIRES_FRAME = 1,
  MODULE_REQUIRES_TWOFRAMES = 2,
  MODULE_REQUIRES_SIDEBAR = 4,
};

#define REGISTER_MODULE(X)                      \
    {                                           \
      if( USE_##X )                             \
      {                                         \
        X *pMod = new X();                      \
        appendModule( pMod );                   \
      }                                         \
    }

typedef struct __PlaYUVerModuleDefinition
{
  int   m_iModuleType;
  const char* m_pchModuleCategory;
  const char* m_pchModuleName;
  const char* m_pchModuleTooltip;
  bool  m_bRequiresNewWindow;
}PlaYUVerModuleDefinition;

class SubWindowHandle;

class PlaYUVerModuleIf
{
  friend class ModulesHandle;

public:
  PlaYUVerModuleDefinition m_cModuleDef;
private:
  QAction* m_pcAction;
  SubWindowHandle* m_pcSubWindow;
  SubWindowHandle* m_pcDisplaySubWindow;

public:
  PlaYUVerModuleIf() {  }
  virtual ~PlaYUVerModuleIf() { }

  virtual Void create() {};
  virtual Void create( PlaYUVerFrame* ) {};

  virtual Void            process() {};
  virtual PlaYUVerFrame*  process( PlaYUVerFrame* )                   { return NULL; };
  virtual PlaYUVerFrame*  process( PlaYUVerFrame* , PlaYUVerFrame* )  { return NULL; };

  virtual Void destroy() {};

};

}  // NAMESPACE

#endif // __PLAYUVERMODULESIF_H__

