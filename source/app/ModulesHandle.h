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
 * \file     ModulesHandle.h
 * \brief    PlaYUVer modules handle
 */

#ifndef __MODULESHANDLE_H__
#define __MODULESHANDLE_H__

#include "config.h"

#include <iostream>
#include <cstdio>

#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif

#include "TypeDef.h"
#include "PlaYUVerModuleIf.h"

namespace plaYUVer
{

class ModulesHandle : public QObject
{
 Q_OBJECT
public:
  ModulesHandle( QWidget * parent = 0 );
  ~ModulesHandle();

  QMenu* createMenus( QMenuBar *MainAppMenuBar );
  Void updateMenus( Bool hasSubWindow );
  PlaYUVerModuleIf* getSelectedModuleIf();

private:
  UInt m_uiModulesCount;
  Int m_uiModuleSelected;
  QMenu* m_pcModulesMenu;
  QList<QMenu*> m_pcModulesSubMenuList;
  QList<PlaYUVerModuleIf*> m_pcPlaYUVerModules;
  QSignalMapper* m_pcActionMapper;
  QVector<QAction*> m_arrayModulesActions;

  Void appendModule( PlaYUVerModuleIf* );
  Void ModulesList( Bool bCreate );
private Q_SLOTS:
  void selectModule( int index );
};

}  // NAMESPACE

#endif // __MODULESHANDLE_H__

