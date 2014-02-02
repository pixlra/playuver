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

#include <QMainWindow>
#include "TypeDef.h"

class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;

namespace plaYUVer
{

class ImageInterface;

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
  void saveAs();

  //! View functions
  void zoomIn();
  void zoomOut();
  void normalSize();
  void zoomToFit();

  //!  Show a message box with some information about the plaYUVerApp App
  void about();
  void updateMenus();

  void updateWindowMenu();

  ImageInterface *createImageInterface();
  void addImageInterface( ImageInterface *child );
  void setActiveImageInterface( QWidget *window );

private:
  Void createActions();
  Void createMenus();
  Void createToolBars();
  Void createStatusBar();
  Void readSettings();
  Void writeSettings();
  ImageInterface *activeImageInterface();
  QMdiSubWindow *findImageInterface( const QString &fileName );

  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  Void scaleImage( Double factor );

  QString m_cLastOpenPath;

  QMdiArea *mdiArea;
  QSignalMapper *windowMapper;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *windowMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *viewToolBar;
  QToolBar *videoToolBar;

  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *exitAct;
  QAction *closeAct;
  QAction *closeAllAct;

  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *normalSizeAct;
  QAction *zoomToFitAct;

  QAction *tileAct;
  QAction *cascadeAct;
  QAction *nextAct;
  QAction *previousAct;
  QAction *separatorAct;

  // Video actions.
  QAction *acVideoPlay;
  QAction *acVideoPause;
  QAction *acVideoStop;
  QAction *acVideoBackward;
  QAction *acVideoForward;
  QAction *acVideoLoop;
  QAction *acVideoLock;
  QAction *acVideoInterlace;
  QAction *acVideoCenter;

  QAction *aboutAct;
  QAction *aboutQtAct;

};

} // NAMESPACE

#endif // __PLAYUVERAPP_H__
