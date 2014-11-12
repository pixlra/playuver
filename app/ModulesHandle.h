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
 * \file     ModulesHandle.h
 * \brief    PlaYUVer modules handle
 */

#ifndef __MODULESHANDLE_H__
#define __MODULESHANDLE_H__

#include "config.h"
#include "PlaYUVerDefs.h"

#include <iostream>
#include <cstdio>

#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "PlaYUVerAppModuleIf.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class PlaYUVerAppModuleIf;
class SubWindowHandle;

class ModulesHandle: public QObject
{
Q_OBJECT
public:
  ModulesHandle( QMainWindow* parent = 0, QMdiArea *mdiArea = 0 );
  ~ModulesHandle();

  QMenu* createMenus( QMenuBar* MainAppMenuBar );
  Void updateMenus( Bool hasSubWindow );
  SubWindowHandle* processModuleHandlingOpt();

  static Void destroyModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );
  static Bool applyModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf, Bool isPlaying, Bool disableThreads = false );
  static Void swapModulesWindowsIf( PlaYUVerAppModuleIf *pcCurrModuleIf );
  static Bool showModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf, PlaYUVerFrame* processedFrame );
  static Bool showModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf, Double moduleResult );
  static Void applyAllModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );

private:
  QMainWindow *m_pcParent;
  QMdiArea *m_pcMdiArea;
  UInt m_uiModulesCount;
  Int m_iOptionSelected;

  enum
  {
    INVALID_OPT = -1,
    SWAP_FRAMES_OPT = -2,
    APPLY_ALL_OPT = -3,
  };

  QMenu* m_pcModulesMenu;
  QList<QMenu*> m_pcModulesSubMenuList;
  QList<PlaYUVerAppModuleIf*> m_pcPlaYUVerModules;
  QSignalMapper* m_pcActionMapper;
  QVector<QAction*> m_arrayModulesActions;

  enum MODULES_ACTION_LIST
  {
    FORCE_NEW_WINDOW_ACT = 0,
    FORCE_PLAYING_REFRESH_ACT,
    APPLY_ALL_ACT,
    SWAP_FRAMES_ACT,
    DISABLE_ALL_ACT,
    MODULES_TOTAL_ACT
  };
  QVector<QAction*> m_arrayActions;

  Void appendModule( PlaYUVerModuleIf* pModuleIf );
  SubWindowHandle* enableModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );
  Void openModuleIfStream( PlaYUVerAppModuleIf *pcCurrModuleIf );

  Void customEvent( QEvent *event );

private Q_SLOTS:
  void selectModule( int index );
  void destroyAllModulesIf();

};

}  // NAMESPACE

#endif // __MODULESHANDLE_H__

