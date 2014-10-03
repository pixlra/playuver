/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
#include "PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QMainWindow>
#include "SubWindowHandle.h"
#include "ModulesHandle.h"
#include "PropertiesSidebar.h"
#include "QualityMeasurementSidebar.h"
#include "AboutDialog.h"

namespace plaYUVer
{

class PlaYUVerStream;

class plaYUVerApp: public QMainWindow
{
Q_OBJECT

public:
  plaYUVerApp();
  Void parseArgs( Int argc, Char *argv[] );

  enum RecentFileList
  {
    MAXRECENTFILES = 2,
  };

protected:
  Void closeEvent( QCloseEvent *event );

private Q_SLOTS:

  //! File functions
  void open();
  void save();
  void format();
  void reload();
  void reloadAll();
  void loadAll();
  void closeActiveWindow();
  void closeAll();

  //! Playing functions
  void play();
  void pause();
  void stop();
  void playEvent();
  void seekSliderEvent( int new_frame_num );
  void seekEvent( int direction );
  void lockButtonEvent();
  void videoSelectionButtonEvent();

  void ModuleHandling( QAction * );

  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 120 scale the image up by 20% and
   *        80 scale the image down by 25%
   */
  void scaleFrame( int ratio = 100.0 );
  void setZoomFromSBox( double ratio );

  void normalSize();
  void zoomToFit();

  //!  Show a message box with some information about the plaYUVerApp App
  void about();

  void chageSubWindowSelection();
  void updateWindowMenu();

  //! Drag and drop functions
  void dragEnterEvent( QDragEnterEvent *event );
  void dropEvent( QDropEvent *event );

  void setActiveSubWindow( QWidget *window );

  void updatePixelValueStatusBar( const QPoint & pos, PlaYUVerFrame* frame );

  void updateProperties();
  void updatePropertiesSelectedArea( QRect area );

  void updateZoomFactorSBox();

  void setAllSubWindowTool();
  void setNavigationTool();
  void setSelectionTool();

private:

  class PlaYUVerMdiArea: public QMdiArea
  {
  public:
    PlaYUVerMdiArea( QWidget *parent = 0 ) :
            QMdiArea( parent ),
            m_pixmapLogo( ":/images/playuver-backgroud-logo.png" )
    {
    }
  protected:
    void paintEvent( QPaintEvent *event )
    {
      QMdiArea::paintEvent( event );
      QPainter painter( viewport() );
      QSize logoSize = 2 * size() / 3;

      QPixmap pixFinalLogo = m_pixmapLogo.scaled(logoSize,Qt::KeepAspectRatio);

      // Calculate the logo position - the bottom right corner of the mdi area.
      int x = width() / 2 - pixFinalLogo.width() / 2;
      int y = height() / 2 - pixFinalLogo.height() / 2;
      painter.drawPixmap( x, y, pixFinalLogo );
    }
  private:
    // Store the logo image.
    QPixmap m_pixmapLogo;
  };

  //QMdiArea *mdiArea;
  PlaYUVerMdiArea *mdiArea;
  SubWindowHandle *m_pcCurrentSubWindow;
  ModulesHandle *m_pcModulesHandle;

  QString m_cLastOpenPath;
  QTimer *m_pcPlayingTimer;
  Bool m_bIsPlaying;
  QTime m_cTimer;
  UInt m_uiAveragePlayInterval;

  Void loadFile( QString fileName );

  Void readRecentFileList();
  Void writeRecentFileList();

  Void zoomToFitAll();

  //! Playing functions
  UInt64 getMaxFrameNumber();
  Void startPlay();
  Void setTimerStatus();
  Void updateCurrFrameNum();
  Void updateTotalFrameNum();

  Void updateMenus();
  Void updateStreamProperties();
  Void updateFrameProperties();

  Void createActions();
  Void createMenus();
  Void createToolBars();
  Void createDockWidgets();
  Void createStatusBar();

  Void readSettings();
  Void writeSettings();

  SubWindowHandle *activeSubWindow();

  static SubWindowHandle* findSubWindow( const QMdiArea* mdiArea, const QString& fileName );
  static SubWindowHandle* findSubWindow( const QMdiArea* mdiArea, const SubWindowHandle* subWindow );

  QVector<SubWindowHandle*> m_acPlayingSubWindows;
  QSlider *m_pcFrameSlider;
  QLabel *m_pcCurrFrameNumLabel;
  QLabel *m_pcTotalFrameNumLabel;
  QDoubleSpinBox *m_pcZoomFactorSBox;

  QSignalMapper *mapperZoom;
  QSignalMapper *mapperSeekVideo;
  QSignalMapper *mapperWindow;

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
    VIEW_MENU,
    TOOLS_MENU,
    VIDEO_MENU,
    DOCK_VIEW_MENU,
    WINDOW_MENU,
    ABOUT_MENU,
    TOTAL_MENUS,
  };
  QVector<QMenu*> m_arrayMenu;

  enum SIDEBAR_LIST
  {
    STREAM_DOCK = 0,
    FRAME_DOCK,
    QUALITY_DOCK,
    TOTAL_DOCK,
  };
  QVector<QDockWidget*> m_arraySideBars;

  StreamPropertiesSideBar* m_pcStreamProperties;
  FramePropertiesSideBar* m_pcFrameProperties;
  QualityMeasurementSidebar* m_pcQualityMeasurement;

  enum TOOLBAR_LIST
  {
    FILE_TOOLBAR = 0,
    VIEW_TOOLBAR,
    VIDEO_TOOLBAR,
    TOTAL_TOOLBAR,
  };
  QVector<QToolBar*> m_arrayToolBars;

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
    ZOOM_FIT_ACT,
    ZOOM_NORMAL_ACT,
    PLAY_ACT,
    /*PAUSE_ACT,*/
    STOP_ACT,
    VIDEO_FORWARD_ACT,
    VIDEO_BACKWARD_ACT,
    VIDEO_LOOP_ACT,
    VIDEO_LOCK_ACT,
    VIDEO_LOCK_SELECTION_ACT,
    NAVIGATION_TOOL_ACT,
    SELECTION_TOOL_ACT,
    TILE_WINDOWS_ACT,
    CASCADE_WINDOWS_ACT,
    PREVIOUS_WINDOWS_ACT,
    NEXT_WINDOWS_ACT,
    SEPARATOR_ACT,
    UPDATE_ACT,
    ABOUT_ACT,
    ABOUTQT_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QAction *actionPopupMenu;

  // Tools Actions;
  QActionGroup *actionGroupTools;
  QAction *actionNavigationTool;
  QAction *actionSelectionTool;
  enum eTool m_appTool;

  PlaYUVerRecentFileListInfo m_aRecentFileStreamInfo;

  AboutDialog* m_pcAboutDialog;

};

}  // NAMESPACE

#endif // __PLAYUVERAPP_H__
