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
 * \file     PlaYUVerSubWindowHandle.h
 * \brief    Handle multiple sub-windows
 */

#ifndef __PLAYUVERSUBWINDOWHANDLE_H__
#define __PLAYUVERSUBWINDOWHANDLE_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif

namespace plaYUVer
{

class SubWindowHandle;
class PlaYUVerMdiArea;
class PlaYUVerMdiSubWindow;

class PlaYUVerSubWindowHandle: public QWidget
{
Q_OBJECT

public:
  PlaYUVerSubWindowHandle( QWidget *parent );

  Void addSubWindow( SubWindowHandle *widget, Qt::WindowFlags flags = 0 );

  SubWindowHandle *activeSubWindow() const;

  QList<SubWindowHandle*> findSubWindow( const QString &aName = QString() ) const;
  QList<SubWindowHandle*> findSubWindow( const UInt uiCategory ) const;
  SubWindowHandle* findSubWindow( const SubWindowHandle* subWindow ) const;

  Void createActions();
  QMenu* createMenu();

  Void readSettings();
  Void writeSettings();

  enum WindowMode
  {
    NormalSubWindows = 0,
    MdiWSubWindows = 1,
  };

private:
  UInt m_uiWindowMode;
  QList<SubWindowHandle*> m_apcSubWindowList;

  QHBoxLayout* m_pcWindowManagerLayout;
  PlaYUVerMdiArea* m_pcMdiArea;
  QList<PlaYUVerMdiSubWindow*> m_apcMdiSubWindowList;

  SubWindowHandle* m_pcActiveWindow;

  enum
  {
    CLOSE_ACT,
    CLOSE_ALL_ACT,
    TILE_WINDOWS_ACT,
    CASCADE_WINDOWS_ACT,
    NEXT_WINDOWS_ACT,
    PREVIOUS_WINDOWS_ACT,
    SEPARATOR_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QSignalMapper* m_mapperWindow;
  QMenu* m_pcMenuWindow;

  /**
   * Internal functions
   */
  Void resetWindowMode();
  Void setWindowMode( UInt uiWindowMode );
  Void removeSubWindow( Int windowIdx );

public Q_SLOTS:
  void updateActiveSubWindow( SubWindowHandle *window = 0 );
  void setActiveSubWindow( QWidget *window );
  void removeSubWindow( SubWindowHandle *window );
  void removeMdiSubWindow( PlaYUVerMdiSubWindow* window );
  void removeActiveSubWindow();
  void removeAllSubWindow();
  void tileSubWindows();
  void cascadeSubWindows();
  void updateMenu();

Q_SIGNALS:
  void changed();
  void windowActivated();

};

}  // NAMESPACE

#endif // __PLAYUVERSUBWINDOWHANDLE_H__
