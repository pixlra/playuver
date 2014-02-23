/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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

typedef struct __PlaYUVerModuleDefinition
{
  const char *m_pchModuleType;
  const char *m_pchModuleName;
  const char *m_pchModuleTooltip;
}PlaYUVerModuleDefinition;


class PlaYUVerModuleIf
{
public:
  PlaYUVerModuleIf()
  {
    m_pcAction = NULL;
  }
  virtual ~PlaYUVerModuleIf()
  {
  }

  QAction* m_pcAction;

  PlaYUVerModuleDefinition m_cModuleDef;

  virtual Void create() {};
  virtual Void process() {};

  virtual Void create( PlaYUVerFrame* ) {};
  virtual PlaYUVerFrame* process( PlaYUVerFrame* ) {};

  virtual Void destroy() {};

};

}  // NAMESPACE

#endif // __PLAYUVERMODULESIF_H__

