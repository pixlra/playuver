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

#include "lib/PlaYUVerDefs.h"
#include <QtGui>
#include "PlaYUVerSubWindowHandle.h"
#include "VideoHandle.h"
#include "DialogSubWindowSelector.h"

namespace plaYUVer
{

VideoHandle::VideoHandle( QWidget* parent, PlaYUVerSubWindowHandle* windowManager ) :
        m_pcParet( parent ),
        m_pcMainWindowManager( windowManager )
{
  m_pcCurrentVideoSubWindow = NULL;
  m_bIsPlaying = false;
  m_pcPlayingTimer = new QTimer;
#if( QT_VERSION_PLAYUVER == 5 )
  m_pcPlayingTimer->setTimerType( Qt::CoarseTimer );
#endif
  connect( m_pcPlayingTimer, SIGNAL( timeout() ), this, SLOT( playEvent() ) );
  m_acPlayingSubWindows.clear();
}

VideoHandle::~VideoHandle()
{

}

Void VideoHandle::createActions()
{

  m_arrayActions.resize( TOTAL_ACT );
  m_arrayActions[PLAY_ACT] = new QAction( "Play", this );
  m_arrayActions[PLAY_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPlay ) ) );
  m_arrayActions[PLAY_ACT]->setShortcut( Qt::Key_Space );
  connect( m_arrayActions[PLAY_ACT], SIGNAL( triggered() ), this, SLOT( play() ) );

  m_arrayActions[STOP_ACT] = new QAction( "Stop", this );
  m_arrayActions[STOP_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaStop ) ) );
  connect( m_arrayActions[STOP_ACT], SIGNAL( triggered() ), this, SLOT( stop() ) );

  m_mapperVideoSeek = new QSignalMapper( this );
  connect( m_mapperVideoSeek, SIGNAL( mapped(int) ), this, SLOT( seekEvent(int) ) );

  m_arrayActions[VIDEO_BACKWARD_ACT] = new QAction( "Video Backward", this );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekBackward ) ) );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setShortcut( Qt::Key_Left );
  connect( m_arrayActions[VIDEO_BACKWARD_ACT], SIGNAL( triggered() ), m_mapperVideoSeek, SLOT( map() ) );
  m_mapperVideoSeek->setMapping( m_arrayActions[VIDEO_BACKWARD_ACT], 0 );

  m_arrayActions[VIDEO_FORWARD_ACT] = new QAction( "Video Forward", this );
  m_arrayActions[VIDEO_FORWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  m_arrayActions[VIDEO_FORWARD_ACT]->setShortcut( Qt::Key_Right );
  connect( m_arrayActions[VIDEO_FORWARD_ACT], SIGNAL( triggered() ), m_mapperVideoSeek, SLOT( map() ) );
  m_mapperVideoSeek->setMapping( m_arrayActions[VIDEO_FORWARD_ACT], 1 );

  m_arrayActions[VIDEO_LOOP_ACT] = new QAction( "Repeat", this );
  m_arrayActions[VIDEO_LOOP_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_LOOP_ACT]->setChecked( false );

  m_arrayActions[VIDEO_ZOOM_LOCK_ACT] = new QAction( "Video Zoom Lock", this );
  m_arrayActions[VIDEO_ZOOM_LOCK_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_ZOOM_LOCK_ACT]->setChecked( false );

  m_arrayActions[VIDEO_LOCK_SELECTION_ACT] = new QAction( "Video Lock Window Selection", this );
  connect( m_arrayActions[VIDEO_LOCK_SELECTION_ACT], SIGNAL( triggered() ), this, SLOT( videoSelectionButtonEvent() ) );

  m_pcFrameSlider = new QSlider;
  m_pcFrameSlider->setOrientation( Qt::Horizontal );
  m_pcFrameSlider->setMaximumWidth( 100 );
  m_pcFrameSlider->setMaximumWidth( /* 300 */2000 );
  m_pcFrameSlider->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  m_pcFrameSlider->setEnabled( false );
  connect( m_pcFrameSlider, SIGNAL( sliderMoved(int) ), this, SLOT( seekSliderEvent(int) ) );
}

QMenu* VideoHandle::createMenu()
{
  m_pcMenuVideo = new QMenu( "Video", this );
  m_pcMenuVideo->addAction( m_arrayActions[PLAY_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[STOP_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[VIDEO_LOOP_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[VIDEO_ZOOM_LOCK_ACT] );
  m_pcMenuVideo->addAction( m_arrayActions[VIDEO_LOCK_SELECTION_ACT] );
  return m_pcMenuVideo;
}

QToolBar* VideoHandle::createToolBar()
{
  m_toolbarVideo = new QToolBar( tr( "Video" ) );
  m_toolbarVideo->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
  m_toolbarVideo->addAction( m_arrayActions[PLAY_ACT] );
  m_toolbarVideo->addAction( m_arrayActions[STOP_ACT] );

  QLabel* pcLockLabel = new QLabel( "PlayingLock", this );
  m_arrayActions[VIDEO_LOCK_ACT] = m_toolbarVideo->addWidget( pcLockLabel );
  m_arrayActions[VIDEO_LOCK_ACT]->setVisible( false );

  m_toolbarVideo->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  m_toolbarVideo->addWidget( m_pcFrameSlider );
  m_toolbarVideo->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );
  m_toolbarVideo->addWidget( new QLabel );
  m_pcFrameNumInfo = new WidgetFrameNumber;
  m_toolbarVideo->addWidget( m_pcFrameNumInfo );

  return m_toolbarVideo;
}

QDockWidget* VideoHandle::createDock()
{
  m_pcFramePropertiesSideBar = new FramePropertiesSideBar( this, &m_bIsPlaying );
  m_pcFramePropertiesDock = new QDockWidget( tr( "Frame Information" ), this );
  m_pcFramePropertiesDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_pcFramePropertiesDock->setWidget( m_pcFramePropertiesSideBar );
  connect( m_pcFramePropertiesDock, SIGNAL( visibilityChanged(bool) ), this, SLOT( update() ) );

  return m_pcFramePropertiesDock;
}

QWidget* VideoHandle::createStatusBarMessage()
{
  QWidget* pcStatusBarWidget = new QWidget;
  QHBoxLayout* mainlayout = new QHBoxLayout;

  m_pcVideoFormatLabel = new QLabel;
  m_pcVideoFormatLabel->setText( " " );
  m_pcVideoFormatLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  m_pcVideoFormatLabel->setMinimumWidth( 150 );
  m_pcVideoFormatLabel->setAlignment( Qt::AlignCenter );

  m_pcResolutionLabel = new QLabel;
  m_pcResolutionLabel->setText( " " );
  m_pcResolutionLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  m_pcResolutionLabel->setMinimumWidth( 90 );
  m_pcResolutionLabel->setAlignment( Qt::AlignCenter );

  mainlayout->addWidget( m_pcVideoFormatLabel );
  mainlayout->addWidget( m_pcResolutionLabel );
  pcStatusBarWidget->setLayout( mainlayout );

  return pcStatusBarWidget;
}

Void VideoHandle::updateMenus()
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  Bool hasSubWindow = pcSubWindow ? true : false;

  m_arrayActions[PLAY_ACT]->setEnabled( hasSubWindow );
  //m_arrayActions[PAUSE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[STOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_FORWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_ZOOM_LOCK_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOCK_SELECTION_ACT]->setEnabled( hasSubWindow );
  m_pcFrameSlider->setEnabled( hasSubWindow );
  if( !hasSubWindow )
  {
    m_pcFrameSlider->setValue( 0 );
    m_pcFrameNumInfo->clear();
  }
}

Void VideoHandle::readSettings()
{
  QSettings appSettings;
  m_arrayActions[VIDEO_LOOP_ACT]->setChecked( appSettings.value( "VideoHandle/Repeat", false ).toBool() );
  if( !appSettings.value( "VideoHandle/FrameProperties", true ).toBool() )
    m_pcFramePropertiesDock->close();
}

Void VideoHandle::writeSettings()
{
  QSettings appSettings;
  appSettings.setValue( "VideoHandle/Repeat", m_arrayActions[VIDEO_LOOP_ACT]->isChecked() );
  appSettings.setValue( "VideoHandle/FrameProperties", m_pcFramePropertiesSideBar->isVisible() );
}

Void VideoHandle::update()
{
  if( m_pcCurrentVideoSubWindow )
  {
    PlaYUVerStream* pcStream = m_pcCurrentVideoSubWindow->getInputStream();
    PlaYUVerFrame* pcFrame = m_pcCurrentVideoSubWindow->getCurrFrame();

    Int frame_num = 0;
    UInt64 total_frame_num = 1;

    m_pcVideoFormatLabel->setText( m_pcCurrentVideoSubWindow->getStreamInformation() );

    QString resolution;
    if( pcFrame )
    {
      resolution.append( QString( "%1x%2" ).arg( pcFrame->getWidth() ).arg( pcFrame->getHeight() ) );
    }

    if( pcStream )
    {
      frame_num = pcStream->getCurrFrameNum();
      m_pcFrameNumInfo->setCurrFrameNum( frame_num );
      total_frame_num = getMaxFrameNumber();
      resolution.append( QString( "@%1" ).arg( pcStream->getFrameRate() ) );
    }
    else
    {
      m_pcFrameNumInfo->setCurrFrameNum( 0 );
      if( m_pcCurrentVideoSubWindow->getIsModule() )
        m_pcVideoFormatLabel->setText( QString( "Module" ) );
    }

    if( pcFrame )
      m_pcResolutionLabel->setText( resolution );

    m_pcFramePropertiesSideBar->setData( pcFrame, m_pcCurrentVideoSubWindow->isPlaying() );

    m_pcFrameSlider->setValue( frame_num );
    m_pcFrameSlider->setMaximum( total_frame_num - 1 );
    m_pcFrameNumInfo->setTotalFrameNum( total_frame_num );

    if( m_pcCurrentVideoSubWindow->isPlaying() )
      m_arrayActions[PLAY_ACT]->setIcon( style()->standardIcon( QStyle::SP_MediaPause ) );
    else
      m_arrayActions[PLAY_ACT]->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );

  }
  else
  {
    m_pcFramePropertiesSideBar->setData( NULL, false );
    m_pcFrameSlider->setValue( 0 );
    m_pcFrameNumInfo->clear();
  }
}

Void VideoHandle::update( VideoSubWindow* currSubWindow )
{
  m_pcCurrentVideoSubWindow = currSubWindow;
  update();
}

Void VideoHandle::updateSelectionArea( QRect area )
{
  if( m_pcCurrentVideoSubWindow )
  {
    m_pcFramePropertiesSideBar->setSelection( area );
  }
}

Void VideoHandle::closeSubWindow( VideoSubWindow* currSubWindow )
{
  if( m_acPlayingSubWindows.contains( currSubWindow ) )
  {
    Int pos = m_acPlayingSubWindows.indexOf( currSubWindow );
    m_acPlayingSubWindows.at( pos )->stop();
    m_acPlayingSubWindows.remove( pos );
    if( m_acPlayingSubWindows.size() < 2 )
    {
      m_arrayActions[VIDEO_LOCK_ACT]->setVisible( false );
    }
  }
}

Void VideoHandle::zoomToFactorAll( const Double scale, const QPoint center )
{
  Double factor;

  if( m_arrayActions[VIDEO_ZOOM_LOCK_ACT]->isChecked() )
  {

    factor = m_pcCurrentVideoSubWindow->getScaleFactor();

    SubWindowHandle *subWindow;
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      subWindow = subWindowList.at( i );
      if( m_pcCurrentVideoSubWindow == subWindow )
        continue;
      else
      {
        subWindow->zoomToFactor( factor, center );
      }
    }
  }
}

Void VideoHandle::moveAllScrollBars( const QPoint offset )
{
  if( m_arrayActions[VIDEO_ZOOM_LOCK_ACT]->isChecked() )
  {
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      if( m_pcCurrentVideoSubWindow == subWindowList.at( i ) )
        continue;
      else
        subWindowList.at( i )->adjustScrollBarByOffset( offset );
    }
  }
}

UInt64 VideoHandle::getMaxFrameNumber()
{
  UInt currFrames;
  UInt64 maxFrames = INT_MAX;
  if( m_pcCurrentVideoSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentVideoSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        currFrames = m_acPlayingSubWindows.at( i )->getInputStream()->getFrameNum();
        if( currFrames < maxFrames )
          maxFrames = currFrames;
      }
    }
    else
    {
      maxFrames = m_pcCurrentVideoSubWindow->getInputStream()->getFrameNum();
    }
  }
  return maxFrames;
}

Void VideoHandle::setTimerStatus()
{
  Bool status = false;
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    status |= m_acPlayingSubWindows.at( i )->isPlaying();
  }
  if( m_pcCurrentVideoSubWindow )
  {
    status |= m_pcCurrentVideoSubWindow->isPlaying();
  }
  if( status )
  {
    if( !m_bIsPlaying )
    {
      m_pcPlayingTimer->start();
      m_bIsPlaying = true;
    }
  }
  else
  {
    m_pcPlayingTimer->stop();
    m_bIsPlaying = false;
  }
}

Void VideoHandle::play()
{
  if( !m_pcCurrentVideoSubWindow )
    return;

  if( !m_pcCurrentVideoSubWindow->getInputStream() )
    return;

  if( !m_pcCurrentVideoSubWindow->isPlaying() )  // Not playing
  {
    UInt frameRate;
    UInt timeInterval;
    if( m_acPlayingSubWindows.size() < 2 )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->pause();
      }
      m_acPlayingSubWindows.clear();
      m_acPlayingSubWindows.append( m_pcCurrentVideoSubWindow );
    }
    if( m_acPlayingSubWindows.contains( m_pcCurrentVideoSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->play();
      }
      frameRate = m_acPlayingSubWindows.at( 0 )->getInputStream()->getFrameRate();
      timeInterval = ( UInt )( 1000.0 / frameRate + 0.5 );
      m_pcPlayingTimer->setInterval( timeInterval );
    }
  }
  else
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentVideoSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->pause();
      }
    }
    else
    {
      m_pcCurrentVideoSubWindow->pause();
    }
  }
  setTimerStatus();
  emit changed();
  //update();
}

Void VideoHandle::stop()
{
  if( m_acPlayingSubWindows.size() > 0 )
  {
    for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
    {
      m_acPlayingSubWindows.at( i )->stop();
    }
    m_acPlayingSubWindows.clear();
    setTimerStatus();
  }
  else
  {
    if( m_pcCurrentVideoSubWindow )
    {
      m_pcCurrentVideoSubWindow->stop();
    }
  }
  emit changed();
  //update();
  m_arrayActions[VIDEO_LOCK_ACT]->setVisible( false );
}

Void VideoHandle::playEvent()
{
  Bool bEndOfSequence = false;
  try
  {
    for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
    {
      bEndOfSequence |= m_acPlayingSubWindows.at( i )->playEvent();
    }
    if( bEndOfSequence && !m_arrayActions[VIDEO_LOOP_ACT]->isChecked() )
    {
      stop();
    }
  }
  catch( const char *msg )
  {
    QString warningMsg = "Error while playing " + QFileInfo( m_pcCurrentVideoSubWindow->getCurrentFileName() ).fileName() + " with the following error: \n"
        + msg;
    QMessageBox::warning( this, QApplication::applicationName(), warningMsg );
    //statusBar()->showMessage( warningMsg, 2000 );
    qDebug( ) << warningMsg;
    stop();
    m_pcCurrentVideoSubWindow->close();
  }
  emit changed();
  //update();
}

Void VideoHandle::seekEvent( Int direction )
{
  if( m_pcCurrentVideoSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentVideoSubWindow ) )
    {
      if( !( ( UInt )( m_pcCurrentVideoSubWindow->getInputStream()->getCurrFrameNum() + 1 ) >= getMaxFrameNumber() && direction > 0 ) )
      {
        for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
        {
          m_acPlayingSubWindows.at( i )->seekRelativeEvent( direction > 0 ? true : false );
        }
      }
    }
    else
    {
      m_pcCurrentVideoSubWindow->seekRelativeEvent( direction > 0 ? true : false );
    }
    emit changed();
    //update();
  }
}

Void VideoHandle::seekSliderEvent( Int new_frame_num )
{
  if( m_pcCurrentVideoSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentVideoSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->seekAbsoluteEvent( ( UInt )new_frame_num );
      }
    }
    else
    {
      m_pcCurrentVideoSubWindow->seekAbsoluteEvent( ( UInt )new_frame_num );
    }
    emit changed();
    //update();
  }
}

Void VideoHandle::videoSelectionButtonEvent()
{
  DialogSubWindowSelector dialogWindowsSelection( this, m_pcMainWindowManager, SubWindowHandle::VIDEO_STREAM_SUBWINDOW );
  QStringList cWindowListNames;
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    cWindowListNames.append( m_acPlayingSubWindows.at( i )->getWindowName() );
    dialogWindowsSelection.setSubWindowList( cWindowListNames );
  }
  if( dialogWindowsSelection.exec() == QDialog::Accepted )
  {
    QStringList selectedWindows = dialogWindowsSelection.getSelectedWindows();
    m_acPlayingSubWindows.clear();
    VideoSubWindow *videoSubWindow;
    QString windowName;
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_STREAM_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      videoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
      windowName = videoSubWindow->getWindowName();
      if( selectedWindows.contains( windowName ) )
      {
        m_acPlayingSubWindows.append( videoSubWindow );
        videoSubWindow->seekAbsoluteEvent( m_acPlayingSubWindows.at( 0 )->getInputStream()->getCurrFrameNum() );
      }
    }
    if( m_acPlayingSubWindows.size() > 0 )
    {
      m_acPlayingSubWindows.at( 0 )->activateWindow();
      if( m_acPlayingSubWindows.size() > 1 )
      {
        m_arrayActions[VIDEO_LOCK_ACT]->setVisible( true );
      }
      if( m_bIsPlaying )
      {
        for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
        {
          m_acPlayingSubWindows.at( i )->play();
        }
      }
    }
    else
    {
      stop();
    }
    return;
  }
}

}   // NAMESPACE

