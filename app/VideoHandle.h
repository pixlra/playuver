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
 * \file     VideoHandle.h
 * \brief    Class to handle video playback
 */

#ifndef __VIDEOHANDLE_H__
#define __VIDEOHANDLE_H__

#include "config.h"
#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "VideoSubWindow.h"
#include "WidgetFrameNumber.h"
#include "FramePropertiesSidebar.h"

namespace plaYUVer
{

class PlaYUVerSubWindowHandle;
class VideoSubWindow;

class VideoHandle: public QWidget
{
Q_OBJECT
public:
  VideoHandle( QWidget*, PlaYUVerSubWindowHandle * );
  ~VideoHandle();

  Void createActions();
  QMenu* createMenu();
  QToolBar* createToolBar();
  QDockWidget* createDock();
  QWidget* createStatusBarMessage();
  Void updateMenus();

  Void readSettings();
  Void writeSettings();

  Void update( VideoSubWindow* currSubWindow );

  Void closeSubWindow( VideoSubWindow* currSubWindow );

private:
  QWidget* m_pcParet;
  PlaYUVerSubWindowHandle* m_pcMainWindowManager;
  enum
  {
    PLAY_ACT,
    STOP_ACT,
    VIDEO_FORWARD_ACT,
    VIDEO_BACKWARD_ACT,
    VIDEO_LOOP_ACT,
    VIDEO_ZOOM_LOCK_ACT,
    VIDEO_LOCK_ACT,
    VIDEO_LOCK_SELECTION_ACT,
    TOTAL_ACT,
  };
  QVector<QAction*> m_arrayActions;
  QSignalMapper* m_mapperVideoSeek;

  QSlider* m_pcFrameSlider;
  WidgetFrameNumber* m_pcFrameNumInfo;

  QMenu* m_pcMenuVideo;
  QToolBar* m_toolbarVideo;

  QDockWidget* m_pcFramePropertiesDock;
  FramePropertiesSideBar* m_pcFramePropertiesSideBar;

  QLabel* m_pcFormatCodeLabel;
  QLabel* m_pcResolutionLabel;
  QLabel* m_pcColorSpace;

  VideoSubWindow* m_pcCurrentVideoSubWindow;
  QVector<VideoSubWindow*> m_acPlayingSubWindows;

  QTimer *m_pcPlayingTimer;
  Bool m_bIsPlaying;


  UInt64 getMaxFrameNumber();
  Void setTimerStatus();

Q_SIGNALS:
  void changed();

private Q_SLOTS:
  void update();
  void updateSelectionArea( QRect area );
  void zoomToFactorAll( const double factor, const QPoint center = QPoint() );
  void moveAllScrollBars( const QPoint offset );
  void play();
  void stop();
  void playEvent();
  void seekSliderEvent( int new_frame_num );
  void seekEvent( int direction );
  void videoSelectionButtonEvent();

};

}   // NAMESPACE

#endif // __VIDEOHANDLE_H__
