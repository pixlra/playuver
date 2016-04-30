/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     VideoHandle.h
 * \brief    Class to handle video playback
 */

#ifndef __VIDEOHANDLE_H__
#define __VIDEOHANDLE_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"

#include <QWidget>
#include <QMenu>
#include <QVector>
#include <QPoint>

#define _CONTROL_PLAYING_TIME_ 0

class QToolBar;
class QDockWidget;
class QSignalMapper;
class QLabel;
class QSlider;
class QElapsedTimer;


class PlaYUVerSubWindowHandle;
class SubWindowAbstract;
class VideoSubWindow;
class FramePropertiesDock;
class WidgetFrameNumber;

class VideoHandle: public QWidget
{
Q_OBJECT
public:
  VideoHandle( QWidget*, PlaYUVerSubWindowHandle * );
  ~VideoHandle();

  Void createActions();
  QMenu* createVideoMenu();
  QMenu* createImageMenu();
  QToolBar* createToolBar();
  QDockWidget* createDock();
  QWidget* createStatusBarMessage();
  Void updateMenus();

  Void readSettings();
  Void writeSettings();

  Void update( VideoSubWindow* currSubWindow );

  Void openSubWindow( VideoSubWindow* subWindow );

private:
  QWidget* m_pcParet;
  PlaYUVerSubWindowHandle* m_pcMainWindowManager;
  enum
  {
    PLAY_ACT,
    STOP_ACT,
    VIDEO_FORWARD_ACT,
    VIDEO_BACKWARD_ACT,
    VIDEO_GOTO_ACT,
    VIDEO_REPEAT_ACT,
    VIDEO_ZOOM_LOCK_ACT,
    VIDEO_LOCK_ACT,
    VIDEO_LOCK_SELECTION_ACT,
    NAVIGATION_TOOL_ACT,
    SELECTION_TOOL_ACT,
    BLOCK_SELECTION_TOOL_ACT,
    SHOW_GRID_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QSignalMapper* m_mapperVideoSeek;

  QSlider* m_pcFrameSlider;
  WidgetFrameNumber* m_pcFrameNumInfo;

  // Tools Actions;
  QActionGroup *actionGroupTools;
  QSignalMapper *m_mapperTools;
  UInt m_uiViewTool;

  QMenu* m_pcMenuVideo;
  QMenu* m_pcMenuImage;
  QToolBar* m_toolbarVideo;

  QDockWidget* m_pcFramePropertiesDock;
  FramePropertiesDock* m_pcFramePropertiesSideBar;

  QLabel* m_pcVideoFormatLabel;
  QLabel* m_pcResolutionLabel;

  VideoSubWindow* m_pcCurrentVideoSubWindow;
  QVector<VideoSubWindow*> m_acPlayingSubWindows;

  QTimer *m_pcPlayingTimer;
  Bool m_bIsPlaying;

#if( _CONTROL_PLAYING_TIME_ == 1 )
  UInt m_uiNumberPlayedFrames;
  Double m_dAverageFps;
  QElapsedTimer* m_pcPlayControlTimer;
#endif

  UInt64 getMaxFrameNumber();
  Void setTimerStatus();

Q_SIGNALS:
  void changed();

private Q_SLOTS:
  void update();
  void updateSelectionArea( QRect area );
  void closeSubWindow( SubWindowAbstract* subWindow );
  void zoomToFactorAll( const double factor, const QPoint center = QPoint() );
  void moveAllScrollBars( const QPoint offset );
  void play();
  void stop();
  void playEvent();
  void seekSliderEvent( int new_frame_num );
  void seekEvent( int direction );
  void seekVideo();
  void videoSelectionButtonEvent();
  void setTool( int tool );
  void toggleGrid( bool checked );

};

#endif // __VIDEOHANDLE_H__
