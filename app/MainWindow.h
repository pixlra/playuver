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
 * \file     MainWindow.cpp
 * \brief    Definition of the main window
 * \ingroup  CalypApp
 *
 * @defgroup CalypApp Calyp App (GUI)
 * @{
 *
 * CalypApp is an high level interface providing a GUI
 * based on the Qt libs in order to display provide important
 * information about video streams.
 *
 * @}
 *
 */

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include "CommonDefs.h"
#include "VideoSubWindow.h"
#include "config.h"
#ifdef USE_QTDBUS
#include "DBusAppAdaptor.h"
#endif

#include <QMainWindow>
#include <QString>
#include <QVector>

class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QDoubleSpinBox;
class QActionGroup;
class QAction;
class QSignalMapper;

class CalypOptions;
class CalypStream;
class SubWindowHandle;
class VideoHandle;
class QualityHandle;
class ModulesHandle;
class DBusAppAdaptor;
class VideoSubWindow;
class SubWindowAbstract;
class AboutDialog;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  bool parseArgs( int argc, char* argv[] );
  void loadFile( QString fileName, CalypFileInfo* pStreamInfo = NULL );

public Q_SLOTS:
  void printMessage( const QString& msg );
  void printMessage( const QString& msg, unsigned int logLevel );

protected:
  void closeEvent( QCloseEvent* event );

private Q_SLOTS:

  void about();

  /**
   *  File functions slots
   */
  void open();
  // void openDevice();
  void openRecent();
  void saveFrame();
  void saveStream();
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

  void dragEnterEvent( QDragEnterEvent* event );
  void dropEvent( QDropEvent* event );

  void update();
  void updateZoomFactorSBox();

private:
  DBusAppAdaptor* m_pDBusAdaptor;
  CalypOptions* m_pcCmdParser;
  /**
   * Save the current subwindow for every category
   */
  SubWindowAbstract* m_pcCurrentSubWindow;  //!< General always set
  VideoSubWindow* m_pcCurrentVideoSubWindow;

  QString m_cLastOpenPath;
  QStringList showFileDialog( bool bRead );

  static VideoSubWindow* findVideoStreamSubWindow( const SubWindowHandle* windowManager,
                                                   const QString& fileName );

  void updateMenus();

  void createActions();
  void createMenus();
  void createToolBars();
  void createDockWidgets();
  void createStatusBar();

  void addStreamInfoToRecentList( CalypFileInfo streamInfo );
  void updateRecentFileActions();
  void checkRecentFileActions();

  void readSettings();
  void writeSettings();

  QDoubleSpinBox* m_pcZoomFactorSBox;

  QSignalMapper* mapperZoom;
  QSignalMapper* mapperWindow;

  /**
   * App modules
   * Video
   * Quality
   * Modules
   */
  VideoHandle* m_appModuleVideo;
  QualityHandle* m_appModuleQuality;
  ModulesHandle* m_appModuleExtensions;
  SubWindowHandle* m_pcWindowHandle;

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
    OPEN_DEVICE_ACT,
    SAVE_ACT,
    SAVE_STREAM_ACT,
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
    UPDATE_ACT,
    ABOUT_ACT,
    ABOUTQT_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;

#define MAX_RECENT_FILES 20
  QVector<QAction*> m_arrayRecentFilesActions;
  CalypFileInfoVector m_aRecentFileStreamInfo;

  AboutDialog* m_pcAboutDialog;
};

#endif  // __MAINWINDOW_H__
