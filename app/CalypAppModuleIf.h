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
 * \file     CalypAppModuleIf.h
 * \brief    Calyp App modules interface
 */

#ifndef __CALYPAPPMODULESIF_H__
#define __CALYPAPPMODULESIF_H__

#include "CommonDefs.h"
#include "ModuleHandleDock.h"
#include "config.h"
#include "lib/CalypFrame.h"
#include "lib/CalypModuleIf.h"
#include "lib/CalypStream.h"

#include <QEvent>
#include <QVector>
#include <cstdio>
#include <iostream>

class QAction;
class QDockWidget;

#define CLP_MODULE_MAX_NUM_FRAMES 5

class VideoSubWindow;

class CalypAppModuleIf
#ifdef CALYP_THREADED_MODULES
    : public QThread
#else
    : public QObject
#endif
{
  friend class ModulesHandle;
  friend class ModuleHandleDock;
  friend class ModulesHandleOptDialog;

private:
  bool m_bIsRunning;
  bool m_bSuccess;

  QAction* m_pcModuleAction;
  CalypModuleIf* m_pcModule;

  VideoSubWindow* m_pcSubWindow[CLP_MODULE_MAX_NUM_FRAMES];

  VideoSubWindow* m_pcDisplaySubWindow;

  QDockWidget* m_pcDockWidget;
  ModuleHandleDock* m_pcModuleDock;

  CalypStream* m_pcModuleStream;
  CalypFrame* m_pcProcessedFrame;
  double m_dMeasurementResult;

public:
  class EventData : public QEvent
  {
  public:
    EventData( bool success, CalypAppModuleIf* module )
        : QEvent( QEvent::User )
    {
      m_bSuccess = success;
      m_pcModule = module;
    }
    bool m_bSuccess;
    CalypAppModuleIf* m_pcModule;
  };

  CalypAppModuleIf( QObject* parent, QAction* action, CalypModuleIf* module );
  virtual ~CalypAppModuleIf() {}
  QList<VideoSubWindow*> getSubWindowList()
  {
    QList<VideoSubWindow*> arraySubWindows;
    for( int i = 0; i < CLP_MODULE_MAX_NUM_FRAMES; i++ )
      if( m_pcSubWindow[i] )
        arraySubWindows.append( m_pcSubWindow[i] );
    return arraySubWindows;
  }

  CalypModuleIf* getModule() { return m_pcModule; }
  unsigned int getModuleRequirements() { return m_pcModule->m_uiModuleRequirements; }
  void update();
  bool apply( bool isPlaying = false, bool disableThreads = false );
  bool isRunning();
  void show();
  void destroy();

protected:
  virtual void run();
};

#endif  // __CALYPAPPMODULESIF_H__
