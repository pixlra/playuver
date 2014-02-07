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

#include <QMainWindow>
#include "TypeDef.h"

class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;

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

private slots:

  //! File functions
  void open();
  void save();

  //! Playing functions
  void play();
  void pause();
  void stop();
  void playEvent();

  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  void scaleFrame( int ratio = 100 );

  void normalSize();
  void zoomToFit();

  //!  Show a message box with some information about the plaYUVerApp App
  void about();
  void updateMenus();

  void updateWindowMenu();

  /*
   * Drag and drop functions
   */
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);

  SubWindowHandle *createSubWindow();
  void addSubWindow( SubWindowHandle *child );
  void setActiveSubWindow( QWidget *window );

private:
  QMdiArea *mdiArea;

  QString m_cLastOpenPath;
  QTimer *playingTimer;

  Void createActions();
  Void createMenus();
  Void createToolBars();
  Void createStatusBar();
  Void readSettings();
  Void writeSettings();
  SubWindowHandle *activeSubWindow();
  QMdiSubWindow *findSubWindow( const QString &fileName );

  QSignalMapper *mapperZoom;
  QSignalMapper *mapperWindow;

  QMenu *menuFile;
  QMenu *menuView;
  QMenu *menuWindow;
  QMenu *menuHelp;

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
