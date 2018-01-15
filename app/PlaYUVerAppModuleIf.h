/*    This file is a part of PlaYUVer project
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
 * \file     PlaYUVerAppModuleIf.h
 * \brief    PlaYUVer App modules interface
 */

#ifndef __PLAYUVERAPPMODULESIF_H__
#define __PLAYUVERAPPMODULESIF_H__

#include "ModuleHandleDock.h"
#include "PlaYUVerAppDefs.h"
#include "config.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerModuleIf.h"
#include "lib/PlaYUVerStream.h"

#include <QEvent>
#include <QVector>
#include <cstdio>
#include <iostream>

class QAction;
class QDockWidget;

//#define PLAYUVER_THREADED_MODULES

class VideoSubWindow;

class PlaYUVerAppModuleIf
#ifdef PLAYUVER_THREADED_MODULES
    : public QThread
#else
    : public QObject
#endif
{
  friend class ModulesHandle;
  friend class ModuleHandleDock;
  friend class ModulesHandleOptDialog;

private:
  Bool m_bIsRunning;
  Bool m_bSuccess;

  QAction* m_pcModuleAction;
  PlaYUVerModuleIf* m_pcModule;

  VideoSubWindow* m_pcSubWindow[MODULE_REQUIRES_MAX_NUM_FRAMES];

  VideoSubWindow* m_pcDisplaySubWindow;

  QDockWidget* m_pcDockWidget;
  ModuleHandleDock* m_pcModuleDock;

  PlaYUVerStream* m_pcModuleStream;
  PlaYUVerFrame* m_pcProcessedFrame;
  Double m_dMeasurementResult;

public:
  class EventData : public QEvent
  {
  public:
    EventData( bool success, PlaYUVerAppModuleIf* module )
        : QEvent( QEvent::User )
    {
      m_bSuccess = success;
      m_pcModule = module;
    }
    Bool m_bSuccess;
    PlaYUVerAppModuleIf* m_pcModule;
  };

  PlaYUVerAppModuleIf( QObject* parent, QAction* action, PlaYUVerModuleIf* module );
  virtual ~PlaYUVerAppModuleIf() {}
  QList<VideoSubWindow*> getSubWindowList()
  {
    QList<VideoSubWindow*> arraySubWindows;
    for( Int i = 0; i < MODULE_REQUIRES_MAX_NUM_FRAMES; i++ )
      if( m_pcSubWindow[i] )
        arraySubWindows.append( m_pcSubWindow[i] );
    return arraySubWindows;
  }

  PlaYUVerModuleIf* getModule() { return m_pcModule; }
  UInt getModuleRequirements() { return m_pcModule->m_uiModuleRequirements; }
  Void update();
  Bool apply( Bool isPlaying = false, Bool disableThreads = false );
  Bool isRunning();
  Void show();
  Void destroy();

protected:
  virtual void run();
};

#endif  // __PLAYUVERAPPMODULESIF_H__
