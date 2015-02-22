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
 * \file     plaYUVerApp.h
 * \brief    Main definition of the plaYUVerApp app
 */

#ifndef __PLAYUVERAPP_H__
#define __PLAYUVERAPP_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#ifdef USE_QTDBUS
#include "PlaYUVerAppAdaptor.h"
#endif
#include <QMainWindow>

#include "SubWindowHandle.h"
#include "AboutDialog.h"
#include "VideoSubWindow.h"

namespace plaYUVer
{

class PlaYUVerStream;
class PlaYUVerSubWindowHandle;
class VideoHandle;
class QualityHandle;
class ModulesHandle;
class PlaYUVerAppAdaptor;

class plaYUVerApp: public QMainWindow
{
Q_OBJECT

public:
  plaYUVerApp();
  Void parseArgs( Int argc, Char *argv[] );
  Void loadFile( QString fileName, PlaYUVerStreamInfo* pStreamInfo = NULL );

protected:
  Void closeEvent( QCloseEvent *event );

private Q_SLOTS:

  void about();

  /**
   *  File functions slots
   */
  void open();
  void openRecent();
  void save();
  void format();
  void reload();
  void reloadAll();
  void loadAll();
  void closeAll();

  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 120 scale the image up by 20% and
   *        80 scale the image down by 25%
   */
  void scaleFrame( int ratio = 100.0 );
  void zoomFromSBox( double ratio );
  void normalSize();
  void zoomToFit();
  void zoomToFitAll();


  void setTool( int idxTool );

  void dragEnterEvent( QDragEnterEvent *event );
  void dropEvent( QDropEvent *event );

  void update();
  void updateZoomFactorSBox();
  void updateStatusBar( const QString& );

private:

  PlaYUVerAppAdaptor* m_pDBusAdaptor;

  /**
   * Save the current subwindow for every category
   */
  SubWindowHandle* m_pcCurrentSubWindow;  //!< General always set
  VideoSubWindow* m_pcCurrentVideoSubWindow;

  QString m_cLastOpenPath;

  static VideoSubWindow* findVideoStreamSubWindow( const PlaYUVerSubWindowHandle* windowManager, const QString& fileName );

  Void updateMenus();

  Void createActions();
  Void createMenus();
  Void createToolBars();
  Void createDockWidgets();
  Void createStatusBar();

  Void addStreamInfoToRecentList( PlaYUVerStreamInfo streamInfo );
  Void updateRecentFileActions();
  Void checkRecentFileActions();

  Void readSettings();
  Void writeSettings();

  QDoubleSpinBox *m_pcZoomFactorSBox;

  QSignalMapper *mapperZoom;
  QSignalMapper *mapperWindow;

  // Tools Actions;
  QActionGroup *actionGroupTools;
  enum APP_TOOLS_ACTION_LIST
  {
    NAVIGATION_TOOL,
    SELECTION_TOOL,
    TOTAL_TOOLS,
  };
  QSignalMapper *m_mapperTools;
  UInt m_uiViewTool;

  /**
   * App modules
   * Video
   * Quality
   * Modules
   */
  VideoHandle* m_appModuleVideo;
  QualityHandle* m_appModuleQuality;
  ModulesHandle *m_appModuleExtensions;
  PlaYUVerSubWindowHandle* m_pcWindowHandle;

  /**
   * Array of menus for the main app
   * @param position description:
   *          0 - File
   *          1 - View
   *          2 - ...
   */
  enum MAIN_APP_MENU_LIST
  {
    FILE_MENU = 0,
    RECENT_MENU,
    VIEW_MENU,
    TOOLS_MENU,
    VIDEO_MENU,
    DOCK_VIEW_MENU,
    WINDOW_MENU,
    ABOUT_MENU,
    TOTAL_MENUS,
  };
  QVector<QMenu*> m_arrayMenu;

  /**
   * Array of side bars for the main app
   */
  enum SIDEBAR_LIST
  {
    TOTAL_DOCK,
  };
  QVector<QDockWidget*> m_arraySideBars;

  /**
   * Array of tool bars for the main app
   */
  enum TOOLBAR_LIST
  {
    FILE_TOOLBAR = 0,
    VIEW_TOOLBAR,
    VIDEO_TOOLBAR,
    TOTAL_TOOLBAR,
  };
  QVector<QToolBar*> m_arrayToolBars;

  /**
   * Array of actions for the main app
   */
  enum APP_ACTION_LIST
  {
    OPEN_ACT = 0,
    SAVE_ACT,
    FORMAT_ACT,
    RELOAD_ACT,
    RELOAD_ALL_ACT,
    LOAD_ALL_ACT,
    CLOSE_ACT,
    CLOSEALL_ACT,
    EXIT_ACT,
    ZOOM_IN_ACT,
    ZOOM_OUT_ACT,
    ZOOM_NORMAL_ACT,
    ZOOM_FIT_ACT,
    ZOOM_FIT_ALL_ACT,
    NAVIGATION_TOOL_ACT,
    SELECTION_TOOL_ACT,
    UPDATE_ACT,
    ABOUT_ACT,
    ABOUTQT_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;

#define MAX_RECENT_FILES 20
  QVector<QAction*> m_arrayRecentFilesActions;
  PlaYUVerStreamInfoVector m_aRecentFileStreamInfo;

  AboutDialog* m_pcAboutDialog;
};

}  // NAMESPACE

#endif // __PLAYUVERAPP_H__
