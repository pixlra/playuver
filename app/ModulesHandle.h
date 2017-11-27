/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     ModulesHandle.h
 * \brief    PlaYUVer modules handle
 */

#ifndef __MODULESHANDLE_H__
#define __MODULESHANDLE_H__

#include "config.h"

#include <cstdio>
#include <iostream>

#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "PlaYUVerAppDefs.h"
#include "PlaYUVerAppModuleIf.h"
#include "lib/PlaYUVerFrame.h"

class VideoHandle;
class PlaYUVerSubWindowHandle;
class PlaYUVerAppModuleIf;
class SubWindowHandle;

class ModulesHandle : public QWidget
{
  Q_OBJECT
public:
  ModulesHandle( QWidget*, PlaYUVerSubWindowHandle*, VideoHandle* );
  ~ModulesHandle();

  Void createActions();
  QMenu* createMenu();
  // QDockWidget* createDock();
  Void buildMenu();
  Void updateMenus();

  Void readSettings();
  Void writeSettings();

  static Void destroyModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );
  static Void applyModuleIf( QList<PlaYUVerAppModuleIf*> pcCurrModuleIfList, Bool isPlaying = false, Bool disableThreads = false );

private:
  QWidget* m_pcParent;
  PlaYUVerSubWindowHandle* m_pcMainWindowManager;
  VideoHandle* m_appModuleVideo;

  enum
  {
    INVALID_OPT = -1,
    SWAP_FRAMES_OPT = 0,
    APPLY_ALL_OPT = 1,
  };

  QMenu* m_pcModulesMenu;
  QList<QMenu*> m_pcModulesSubMenuList;
  QList<PlaYUVerAppModuleIf*> m_pcPlaYUVerAppModuleIfList;

  QVector<QAction*> m_arrayModulesActions;

  enum MODULES_ACTION_LIST
  {
    LOAD_EXTERNAL_ACT,
    // FORCE_PLAYING_REFRESH_ACT,
    APPLY_ALL_ACT,
    SWAP_FRAMES_ACT,
    DISABLE_ACT,
    DISABLE_ALL_ACT,
    FORCE_NEW_WINDOW_ACT,
    MODULES_TOTAL_ACT
  };
  QVector<QAction*> m_arrayActions;
  QSignalMapper* m_pcActionMapper;

  Void enableModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );
  Void applyAllModuleIf( PlaYUVerAppModuleIf* pcCurrModuleIf );
  Void swapModulesWindowsIf( PlaYUVerAppModuleIf* pcCurrModuleIf );

  Void customEvent( QEvent* event );

Q_SIGNALS:
  void changed();

private Q_SLOTS:
  void loadExternalModule();
  void activateModule();
  void processOpt( int index );
  void destroyWindowModules();
  void destroyAllModulesIf();
};

#endif  // __MODULESHANDLE_H__
