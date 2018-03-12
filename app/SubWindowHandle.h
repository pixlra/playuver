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
 * \file     SubWindowHandle.h
 * \brief    Handle multiple sub-windows
 */

#ifndef __SUBWINDOWHANDLE_H__
#define __SUBWINDOWHANDLE_H__

#include "CommonDefs.h"

#include <QString>
#include <QVector>
#include <QWidget>

class QAction;
class QActionGroup;
class QHBoxLayout;
class QMenu;
class QSignalMapper;

class MainWindow;
class SubWindowAbstract;
class MdiArea;
class MdiSubWindow;

class SubWindowHandle : public QWidget
{
  Q_OBJECT

public:
  SubWindowHandle( QWidget* parent );

  void processLogMsg( const QString& msg );

  void addSubWindow( SubWindowAbstract* widget, Qt::WindowFlags flags = 0 );

  SubWindowAbstract* activeSubWindow() const;

  QList<SubWindowAbstract*> findSubWindow( const unsigned int uiCategory = 0 ) const;
  QList<SubWindowAbstract*> findSubWindow( const QString& aName, const unsigned int uiCategory = 0 ) const;
  SubWindowAbstract* findSubWindow( const SubWindowAbstract* subWindow ) const;

  void createActions();
  QMenu* createMenu();

  void readSettings();
  void writeSettings();

  enum WindowMode
  {
    DETACHEDSUBWINDOWMODE = 0,
    MDISUBWINDOWMODE = 1,
  };

private:
  MainWindow* m_pcApp;
  int m_iWindowMode;
  QList<SubWindowAbstract*> m_apcSubWindowList;

  QHBoxLayout* m_pcWindowManagerLayout;
  MdiArea* m_pcMdiArea;
  QList<MdiSubWindow*> m_apcMdiSubWindowList;

  SubWindowAbstract* m_pcActiveWindow;

  QPoint m_cMdiModeWindowPosition;
  QSize m_cMdiModeWindowSize;

  enum
  {
    NORMAL_SUBWINDOW_MODE_ACT,
    MDI_SUBWINDOW_MODE_ACT,
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
  QSignalMapper* m_mapperWindowMode;
  QActionGroup* m_actionGroupWindowMode;
  QSignalMapper* m_mapperWindow;
  QMenu* m_pcMenuWindow;

  /**
   * internal functions
   */
  void addMdiSubWindow( SubWindowAbstract* window );
  void resetWindowMode();
  void removeSubWindow( int windowIdx );

public Q_SLOTS:
  void setWindowMode( int iWindowMode );
  void updateActiveSubWindow( SubWindowAbstract* window = 0 );
  void setActiveSubWindow( QWidget* window );
  void removeSubWindow( SubWindowAbstract* window );
  void removeMdiSubWindow( MdiSubWindow* window );
  void removeActiveSubWindow();
  void removeAllSubWindow();
  void tileSubWindows();
  void cascadeSubWindows();
  void updateMenu();

Q_SIGNALS:
  void changed();
  void windowActivated();
};

#endif  // __SUBWINDOWHANDLE_H__
