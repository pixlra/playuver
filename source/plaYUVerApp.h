/*    This file is a part of plaYUVerApp project
 *    Copyright (C) 2014  by plaYUVerApp developers
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

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QMainWindow>
#include "TypeDef.h"

#include "ModulesHandle.h"

namespace plaYUVer
{

class SubWindowHandle;

class plaYUVerApp: public QMainWindow
{
Q_OBJECT

public:
  plaYUVerApp();

protected:
  Void closeEvent( QCloseEvent *event );

private Q_SLOTS:

  //! File functions
  void open();
  void save();

  //! Playing functions
  void play();
  void pause();
  void stop();
  void playEvent();
  void seekSliderEvent( int new_frame_num );

  void selectModule( QAction * );

  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 120 scale the image up by 20% and
   *        80 scale the image down by 25%
   */
  void scaleFrame( int ratio = 100 );

  void normalSize();
  void zoomToFit();

  //!  Show a message box with some information about the plaYUVerApp App
  void about();

  void chageSubWindowSelection();
  void updateWindowMenu();

  /*
   * Drag and drop functions
   */
  void dragEnterEvent( QDragEnterEvent *event );
  void dropEvent( QDropEvent *event );

  void setActiveSubWindow( QWidget *window );

private:
  QMdiArea *mdiArea;
  ModulesHandle *m_pcModulesHandle;

  QString m_cLastOpenPath;
  QTimer *playingTimer;

  Void updateMenus();

  Void createActions();
  Void createMenus();
  Void createToolBars();
  Void createStatusBar();

  Void readSettings();
  Void writeSettings();

  Void addSubWindow( SubWindowHandle *child );
  SubWindowHandle *createSubWindow();
  SubWindowHandle *activeSubWindow();
  QMdiSubWindow *findSubWindow( const QString &fileName );

  QSlider *m_pcFrameSlider;

  QSignalMapper *mapperZoom;
  QSignalMapper *mapperWindow;

  /**
   * Array of menus for the main app
   * @param position description:
   *          0 - Window
   *          1 - File
   *          2 - View
   *          3 - Help
   */
  enum MAIN_APP_MENU_LIST
  {
    WINDOW_MENU = 0, OTHER_MENU,
  };
  QVector<QMenu*> m_arrayMenu;

  QToolBar *toolbarFile;
  QToolBar *toolbarView;
  QToolBar *toolbarVideo;

  QAction *actionOpen;
  QAction *actionSave;
  QAction *actionExit;
  QAction *actionClose;
  QAction *actionCloseAll;

  QAction *actionZoomIn;
  QAction *actionZoomOut;
  QAction *actionNormalSize;
  QAction *actionZoomToFit;

  QAction *actionTile;
  QAction *actionCascade;
  QAction *actionNext;
  QAction *actionPrevious;
  QAction *actionSeparator;

  // Video actions.
  QAction *actionVideoPlay;
  QAction *actionVideoPause;
  QAction *actionVideoStop;
  QAction *actionVideoBackward;
  QAction *actionVideoForward;
  QAction *actionVideoLoop;
  QAction *actionVideoLock;
  QAction *actionVideoInterlace;
  QAction *actionVideoCenter;

  QAction *actionAbout;
  QAction *actionAboutQt;

};

}  // NAMESPACE

#endif // __PLAYUVERAPP_H__
