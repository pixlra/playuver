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
 * \file     plaYUVerApp.cpp
 * \brief    Main definition of the plaYUVerApp app
 */

#include "plaYUVerApp.h"
#include "SubWindowHandle.h"
#include "InputStream.h"

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QtDebug>

#include "PlaYUVerSettings.h"

namespace plaYUVer
{

plaYUVerApp::plaYUVerApp()
{

  setWindowModality( Qt::ApplicationModal );

  mdiArea = new QMdiArea;
  setCentralWidget( mdiArea );
  //mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

  connect( mdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( chageSubWindowSelection() ) );
  //connect( mdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( updateMenus() ) );

  mapperWindow = new QSignalMapper( this );
  connect( mapperWindow, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

  m_pcModulesHandle = new ModulesHandle( this );

  createActions();
  createToolBars();
  createDockWidgets();
  createMenus();
  createStatusBar();
  updateMenus();

  readSettings();

  setWindowTitle( tr( "plaYUVerApp" ) );
  setWindowIcon( QIcon( ":/images/playuver.png" ) );
  setUnifiedTitleAndToolBarOnMac( true );

  m_bIsPlaying = false;
  playingTimer = new QTimer;
#if( QT_VERSION_PLAYUVER == 5 )
  playingTimer->setTimerType( Qt::CoarseTimer );
#endif
  connect( playingTimer, SIGNAL( timeout() ), this, SLOT( playEvent() ) );

  setAcceptDrops( true );
  mdiArea->setAcceptDrops( true );

  m_pcCurrentSubWindow = NULL;
}

Void plaYUVerApp::closeEvent( QCloseEvent *event )
{
  Int mayCloseAll = true;
  Int msgBoxCloseRet = QMessageBox::Yes;
  SubWindowHandle *imageInterface;

  if( mdiArea->subWindowList().size() >= 1 )
  {
    QMessageBox msgBoxClose( QMessageBox::Question, "PlaYUVer", "There are open files!", QMessageBox::Yes | QMessageBox::No, this );
    msgBoxClose.setDefaultButton( QMessageBox::No );
    msgBoxClose.setInformativeText( "Close all?" );
    msgBoxCloseRet = msgBoxClose.exec();
  }
  switch( msgBoxCloseRet )
  {
  case QMessageBox::Yes:

    mayCloseAll = true;
    for( Int i = 0; i < mdiArea->subWindowList().size(); i++ )
    {
      imageInterface = qobject_cast<SubWindowHandle *>( mdiArea->subWindowList().at( i ) );
      mayCloseAll &= imageInterface->mayClose();
    }

    if( mayCloseAll )
    {
      playingTimer->stop();
      closeAll();
      writeSettings();
      event->accept();
    }
    else
    {
      event->ignore();
    }
    break;
  case QMessageBox::No:
  default:
    event->ignore();
  }
}

// -----------------------  File Functions  -----------------------

void plaYUVerApp::open()
{
  QString supported = tr( "Supported Files" );
  QString formats = InputStream::supportedReadFormats();
  formats.prepend( " (" );
  formats.append( ")" );
  supported.append( formats );  // supported=="Supported Files (*.pbm *.jpg...)"

  QStringList filter;
  filter << supported
         << InputStream::supportedReadFormatsList()
         << tr( "All Files (*)" );

  QString fileName = QFileDialog::getOpenFileName( this, tr( "Open File" ), m_cLastOpenPath, filter.join( ";;" ) );

  if( !fileName.isEmpty() )
  {
    m_cLastOpenPath = QFileInfo( fileName ).path();

    QMdiSubWindow *existing = findSubWindow( fileName );
    if( existing )
    {
      mdiArea->setActiveSubWindow( existing );
      return;
    }

    SubWindowHandle *interfaceChild = new SubWindowHandle( this );  //createSubWindow();
    if( interfaceChild->loadFile( fileName ) )
    {
      mdiArea->addSubWindow( interfaceChild );
      statusBar()->showMessage( tr( "File loaded" ), 2000 );
      interfaceChild->show();
      connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, InputStream *) ), this,
          SLOT( updatePixelValueStatusBar(const QPoint &, InputStream *) ) );
      interfaceChild->zoomToFit();
    }
    else
    {
      interfaceChild->close();
    }
  }
}

void plaYUVerApp::save()
{
  if( m_pcCurrentSubWindow )
    m_pcCurrentSubWindow->save();
}

void plaYUVerApp::format()
{
  if( m_pcCurrentSubWindow )
    m_pcCurrentSubWindow->loadFile( m_pcCurrentSubWindow->currentFile() );
}

void plaYUVerApp::closeActiveWindow()
{
  SubWindowHandle *currSubWindow = activeSubWindow();
  if( currSubWindow )
  {
    if( m_acPlayingSubWindows.contains( currSubWindow ) )
    {
      Int pos = m_acPlayingSubWindows.indexOf( currSubWindow );
      m_acPlayingSubWindows.at( pos )->stop();
      m_acPlayingSubWindows.remove( pos );
    }
  }
  mdiArea->closeActiveSubWindow();
}

void plaYUVerApp::closeAll()
{
  mdiArea->closeAllSubWindows();
}

// -----------------------  Modules Selection  --------------------

void plaYUVerApp::selectModule( QAction *curr_action )
{
  //emit curr_action->triggered();
  if( m_pcCurrentSubWindow )
  {
    PlaYUVerModuleIf* pcCurrMod = m_pcModulesHandle->getSelectedModuleIf();
    if( pcCurrMod )
    {
      m_pcCurrentSubWindow->enableModule( pcCurrMod );
    }
  }
  return;
}

// -----------------------  Update Properties   --------------------

void plaYUVerApp::updateProperties()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcStreamProperties->setData( m_pcCurrentSubWindow->getInputStream() );
    m_pcFrameProperties->setData( m_pcCurrentSubWindow->getInputStream()->getCurrFrame() );
  }
  else
  {
    m_pcStreamProperties->setData( m_pcCurrentSubWindow->getInputStream() );
    m_pcFrameProperties->setData( m_pcCurrentSubWindow->getInputStream()->getCurrFrame() );
  }
}

// -----------------------  Playing Functions  --------------------

Void plaYUVerApp::setTimerStatus()
{
  Bool status = false;
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    status |= m_acPlayingSubWindows.at( i )->isPlaying();
  }
  if( status )
  {
    m_cTimer.start();
  }
  else
  {
    playingTimer->stop();
  }
}

void plaYUVerApp::play()
{
  if( m_pcCurrentSubWindow )
  {
    if( actionVideoLock->isChecked() && m_acPlayingSubWindows.size() )
    {
      Bool bFind = false;
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        if( m_acPlayingSubWindows.at( i ) == m_pcCurrentSubWindow )
        {
          bFind = true;
          break;
        }
      }
      if( !bFind )
      {
        m_acPlayingSubWindows.append( m_pcCurrentSubWindow );
        m_pcCurrentSubWindow->seekAbsoluteEvent( m_acPlayingSubWindows.at( 0 )->getInputStream()->getCurrFrameNum() );
        m_pcCurrentSubWindow->play();
        m_pcFrameSlider->setMaximum( qMin( m_pcFrameSlider->maximum(), ( Int )m_pcCurrentSubWindow->getInputStream()->getFrameNum() - 1 ) );
      }
    }
    else
    {
      playingTimer->stop();
      m_acPlayingSubWindows.clear();
      m_acPlayingSubWindows.append( m_pcCurrentSubWindow );
      m_pcCurrentSubWindow->play();
      m_uiAveragePlayInterval = 0;
      m_bIsPlaying = false;
    }
    if( !m_bIsPlaying )
    {
      UInt frameRate = m_acPlayingSubWindows.at( 0 )->getInputStream()->getFrameRate();
      UInt timeInterval = ( UInt )( 1000.0 / frameRate + 0.5 );
      qDebug( ) << "Desired frame rate: "
                << QString::number( 1000 / timeInterval )
                << " fps";
      playingTimer->start( timeInterval );
      m_cTimer.start();
    }
  }
}

void plaYUVerApp::pause()
{
  if( m_pcCurrentSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->pause();
      }
    }
    else
    {
      m_pcCurrentSubWindow->pause();
    }
    m_pcFrameSlider->setValue( m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() );
  }
  setTimerStatus();
}

void plaYUVerApp::stop()
{
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    m_acPlayingSubWindows.at( i )->stop();
  }
  m_acPlayingSubWindows.clear();
  setTimerStatus();
  m_pcFrameSlider->setValue( m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() );

  if( m_uiAveragePlayInterval )
    qDebug( ) << "Real display time: "
              << QString::number( 1000 / m_uiAveragePlayInterval )
              << " fps";
  m_uiAveragePlayInterval = 0;
}

void plaYUVerApp::playEvent()
{
  UInt time = m_cTimer.elapsed();
  m_cTimer.restart();
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    switch( m_acPlayingSubWindows.at( i )->playEvent() )
    {
    case -1:
      stop();
      m_pcFrameSlider->setValue( m_pcCurrentSubWindow ? m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() : 0 );
      break;
    case -2:
      if( !actionVideoLoop->isChecked() )
      {
        stop();
        m_pcFrameSlider->setValue( m_pcCurrentSubWindow ? m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() : 0 );
        return;
      }
      break;
    case -3:
      m_acPlayingSubWindows.at( i )->close();
      break;
    case -4:
      m_acPlayingSubWindows.remove( i );
      break;
    default:
      break;
    }
  }
  if( m_pcCurrentSubWindow )
    m_pcFrameSlider->setValue( m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() );

  m_uiAveragePlayInterval = ( m_uiAveragePlayInterval + time ) / 2;
}

void plaYUVerApp::seekEvent( int direction )
{
  if( m_pcCurrentSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->seekRelativeEvent( direction > 0 ? true : false );
      }
    }
    else
    {
      m_pcCurrentSubWindow->seekRelativeEvent( direction > 0 ? true : false );
      m_pcFrameSlider->setValue( m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() );
    }
  }
}

void plaYUVerApp::seekSliderEvent( int new_frame_num )
{
  if( m_pcCurrentSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->seekAbsoluteEvent( ( UInt )new_frame_num );
      }
    }
    else
    {
      m_pcCurrentSubWindow->seekAbsoluteEvent( ( UInt )new_frame_num );
      m_pcFrameSlider->setValue( m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum() );
    }
  }
}

void plaYUVerApp::lockButtonEvent()
{
  if( !actionVideoLock->isChecked() )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      stop();
      play();
    }
    else
    {
      stop();
    }
  }
}

// -----------------------  Zoom Functions  -----------------------

void plaYUVerApp::normalSize()
{
  if( activeSubWindow() )
    activeSubWindow()->normalSize();
}

void plaYUVerApp::zoomToFit()
{
  if( activeSubWindow() )
    activeSubWindow()->zoomToFit();
}

void plaYUVerApp::scaleFrame( int ratio )
{
  if( activeSubWindow() )
  {
    activeSubWindow()->scaleViewFactor( ( Double )( ratio ) / 100.0 );

//    actionZoomIn->setEnabled( activeSubWindow()->getScaleFactor() < 3.0 );
//    actionZoomOut->setEnabled( activeSubWindow()->getScaleFactor() > 0.333 );
  }
}

// -----------------------  About Functions  -----------------------

void plaYUVerApp::about()
{
  QMessageBox::about( this, tr( "About plaYUVerApp" ), tr( "The <b>plaYUVerApp</b> is an open-source raw video player " ) );
}

void plaYUVerApp::chageSubWindowSelection()
{
  if( activeSubWindow() )
  {
    if( m_pcCurrentSubWindow )
      m_pcCurrentSubWindow->disableModule();
//    if( m_acPlayingSubWindows.size() < 2 )
//    {
//      playingTimer->stop();
//    }
    m_pcFrameSlider->setMaximum( activeSubWindow()->getInputStream()->getFrameNum() - 1 );
    m_pcFrameSlider->setValue( activeSubWindow()->getInputStream()->getCurrFrameNum() );
    m_pcCurrentSubWindow = activeSubWindow();
    m_pcStreamProperties->setData( m_pcCurrentSubWindow->getInputStream() );
    m_pcFrameProperties->setData( m_pcCurrentSubWindow->getInputStream()->getCurrFrame() );
  }
  else
  {
    m_pcCurrentSubWindow = NULL;
    m_pcStreamProperties->setData( NULL );
    m_pcFrameProperties->setData( NULL );
  }
  updateMenus();
  createStatusBar();
}

// -----------------------  Status bar Functions  -----------------------

void plaYUVerApp::updatePixelValueStatusBar( const QPoint & pos, InputStream* stream )
{
  Pixel sPixelValue;
  Int iWidth, iHeight;
  Int posX = pos.x(), posY = pos.y();
  QString strPixel;
  QString strStatus = QString( "(%1,%2)   " ).arg( posX ).arg( posY );
  PlaYUVerFrame *curFrame = stream->getCurrFrame();

  iWidth = stream->getWidth();
  iHeight = stream->getHeight();

  if( ( posX < iWidth ) && ( posX >= 0 ) && ( posY < iHeight ) && ( posY >= 0 ) )
  {
    sPixelValue = curFrame->getPixelValue( pos, PlaYUVerFrame::COLOR_YUV );
    strPixel = QString( "Y: %1   U: %2   V: %3" ).arg( sPixelValue.Luma ).arg( sPixelValue.ChromaU ).arg( sPixelValue.ChromaV );
    strStatus.append( strPixel );
    statusBar()->showMessage( strStatus, 0 );
  }
  else
  {
    statusBar()->showMessage( " " );
  }
}

// -----------------------  Drag and drop functions  ----------------------

void plaYUVerApp::dragEnterEvent( QDragEnterEvent *event )
{
  //setText(tr("<drop content>"));
  setBackgroundRole( QPalette::Highlight );

  event->acceptProposedAction();
}

void plaYUVerApp::dropEvent( QDropEvent *event )
{

  const QMimeData *mimeData = event->mimeData();

  QList<QUrl> urlList = mimeData->urls();

  if( urlList.size() == 1 )
  {
    QString fileName = urlList.at( 0 ).toLocalFile();

    m_cLastOpenPath = QFileInfo( fileName ).path();

    QMdiSubWindow *existing = findSubWindow( fileName );
    if( !existing )
    {
      SubWindowHandle *interfaceChild = new SubWindowHandle( this );  //createSubWindow();
      if( interfaceChild->loadFile( fileName ) )
      {
        mdiArea->addSubWindow( interfaceChild );
        statusBar()->showMessage( tr( "File loaded" ), 2000 );
        interfaceChild->show();
      }
      else
      {
        interfaceChild->close();
      }
    }
  }
}

// -----------------------  Gui Functions  -----------------------

SubWindowHandle *plaYUVerApp::activeSubWindow()
{
  if( QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow() )
    return qobject_cast<SubWindowHandle *>( activeSubWindow );
  return 0;
}

QMdiSubWindow *plaYUVerApp::findSubWindow( const QString &fileName )
{
  QString canonicalFilePath = QFileInfo( fileName ).canonicalFilePath();

  foreach( QMdiSubWindow * window, mdiArea->subWindowList() ){
  SubWindowHandle *mdiChild = qobject_cast<SubWindowHandle *>( window);
  if( mdiChild->currentFile() == canonicalFilePath )
  return window;
}
  return 0;
}

void plaYUVerApp::setActiveSubWindow( QWidget *window )
{
  if( !window )
    return;
  mdiArea->setActiveSubWindow( qobject_cast<QMdiSubWindow *>( window ) );
}

Void plaYUVerApp::updateMenus()
{
  Bool hasSubWindow = ( activeSubWindow() != 0 );
  m_arrayActions[SAVE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[FORMAT_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[CLOSE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[CLOSEALL_ACT]->setEnabled( hasSubWindow );
  actionTile->setEnabled( hasSubWindow );
  actionCascade->setEnabled( hasSubWindow );
  actionSeparator->setVisible( hasSubWindow );

  actionZoomIn->setEnabled( hasSubWindow );
  actionZoomOut->setEnabled( hasSubWindow );
  actionNormalSize->setEnabled( hasSubWindow );
  actionZoomToFit->setEnabled( hasSubWindow );

  actionVideoPlay->setEnabled( hasSubWindow );
  actionVideoPause->setEnabled( hasSubWindow );
  actionVideoStop->setEnabled( hasSubWindow );
  actionVideoBackward->setEnabled( hasSubWindow );
  actionVideoForward->setEnabled( hasSubWindow );
  actionVideoLoop->setEnabled( hasSubWindow );
  m_pcFrameSlider->setEnabled( hasSubWindow );
  if( !hasSubWindow )
  {
    m_pcFrameSlider->setValue( 0 );
  }

  m_pcModulesHandle->updateMenus( hasSubWindow );
}

void plaYUVerApp::updateWindowMenu()
{
  m_arrayMenu[WINDOW_MENU]->clear();
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[CLOSE_ACT] );
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[CLOSEALL_ACT] );
  m_arrayMenu[WINDOW_MENU]->addSeparator();
  m_arrayMenu[WINDOW_MENU]->addAction( actionTile );
  m_arrayMenu[WINDOW_MENU]->addAction( actionCascade );
  m_arrayMenu[WINDOW_MENU]->addSeparator();
  m_arrayMenu[WINDOW_MENU]->addAction( actionNext );
  m_arrayMenu[WINDOW_MENU]->addAction( actionPrevious );
  m_arrayMenu[WINDOW_MENU]->addAction( actionSeparator );

  updateMenus();

  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  actionSeparator->setVisible( !windows.isEmpty() );
  Int number_windows = windows.size();

  if( number_windows > 1 )
  {
    actionNext->setEnabled( true );
    actionPrevious->setEnabled( true );
  }
  else
  {
    actionNext->setEnabled( false );
    actionPrevious->setEnabled( false );
  }

  for( Int i = 0; i < number_windows; ++i )
  {
    SubWindowHandle *child = qobject_cast<SubWindowHandle *>( windows.at( i ) );

    QString text;
    if( i < 9 )
    {
      text = tr( "&%1 %2" ).arg( i + 1 ).arg( child->userFriendlyCurrentFile() );
    }
    else
    {
      text = tr( "%1 %2" ).arg( i + 1 ).arg( child->userFriendlyCurrentFile() );
    }
    QAction *action = m_arrayMenu[WINDOW_MENU]->addAction( text );
    action->setCheckable( true );
    action->setChecked( child == activeSubWindow() );
    connect( action, SIGNAL( triggered() ), mapperWindow, SLOT( map() ) );
    mapperWindow->setMapping( action, windows.at( i ) );
  }
}

Void plaYUVerApp::createActions()
{
  m_arrayActions.resize( TOTAL_ACT );

  // ------------ File ------------
  m_arrayActions[OPEN_ACT] = new QAction( QIcon( ":/images/open.png" ), tr( "&Open..." ), this );
  m_arrayActions[OPEN_ACT]->setShortcuts( QKeySequence::Open );
  m_arrayActions[OPEN_ACT]->setStatusTip( tr( "Open stream" ) );
  connect( m_arrayActions[OPEN_ACT], SIGNAL( triggered() ), this, SLOT( open() ) );

  m_arrayActions[SAVE_ACT] = new QAction( QIcon( ":/images/save.png" ), tr( "&Save..." ), this );
  m_arrayActions[SAVE_ACT]->setShortcuts( QKeySequence::SaveAs );
  m_arrayActions[SAVE_ACT]->setStatusTip( tr( "Save current frame" ) );
  connect( m_arrayActions[SAVE_ACT], SIGNAL( triggered() ), this, SLOT( save() ) );

  m_arrayActions[FORMAT_ACT] = new QAction( tr( "&Format" ), this );
  m_arrayActions[FORMAT_ACT]->setIcon( QIcon( ":/images/configuredialog.png" ) );
  m_arrayActions[FORMAT_ACT]->setStatusTip( tr( "Open format dialog" ) );
  connect( m_arrayActions[FORMAT_ACT], SIGNAL( triggered() ), this, SLOT( format() ) );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "Cl&ose" ), this );
  m_arrayActions[CLOSE_ACT]->setIcon( QIcon( ":/images/close.png" ) );
  m_arrayActions[CLOSE_ACT]->setStatusTip( tr( "Close the active window" ) );
  connect( m_arrayActions[CLOSE_ACT], SIGNAL( triggered() ), this, SLOT( closeActiveWindow() ) );

  m_arrayActions[CLOSEALL_ACT] = new QAction( tr( "Close &All" ), this );
  m_arrayActions[CLOSEALL_ACT]->setStatusTip( tr( "Close all the windows" ) );
  connect( m_arrayActions[CLOSEALL_ACT], SIGNAL( triggered() ), this, SLOT( closeAll() ) );

  m_arrayActions[EXIT_ACT] = new QAction( tr( "E&xit" ), this );
  m_arrayActions[EXIT_ACT]->setShortcuts( QKeySequence::Quit );
  m_arrayActions[EXIT_ACT]->setStatusTip( tr( "Exit the application" ) );
  connect( m_arrayActions[EXIT_ACT], SIGNAL( triggered() ), qApp, SLOT( closeAllWindows() ) );

  // ------------ View ------------
  mapperZoom = new QSignalMapper( this );
  connect( mapperZoom, SIGNAL( mapped(int) ), this, SLOT( scaleFrame(int) ) );

  actionZoomIn = new QAction( tr( "Zoom &In (25%)" ), this );
  actionZoomIn->setIcon( QIcon( ":/images/zoomin.png" ) );
  actionZoomIn->setShortcut( tr( "Ctrl++" ) );
  actionZoomIn->setStatusTip( tr( "Scale the image up by 25%" ) );
  connect( actionZoomIn, SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( actionZoomIn, 125 );

  actionZoomOut = new QAction( tr( "Zoom &Out (25%)" ), this );
  actionZoomOut->setIcon( QIcon( ":/images/zoomout.png" ) );
  actionZoomOut->setShortcut( tr( "Ctrl+-" ) );
  actionZoomOut->setStatusTip( tr( "Scale the image down by 25%" ) );
  connect( actionZoomOut, SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( actionZoomOut, 80 );

  actionNormalSize = new QAction( tr( "&Normal Size" ), this );
  actionNormalSize->setIcon( QIcon( ":/images/zoomtonormal.png" ) );
  actionNormalSize->setShortcut( tr( "Ctrl+N" ) );
  actionNormalSize->setStatusTip( tr( "Show the image at its original size" ) );
  connect( actionNormalSize, SIGNAL( triggered() ), this, SLOT( normalSize() ) );

  actionZoomToFit = new QAction( tr( "Zoom to &Fit" ), this );
  actionZoomToFit->setIcon( QIcon( ":/images/fittowindow.png" ) );
  actionZoomToFit->setStatusTip( tr( "Zoom in or out to fit on the window." ) );
  actionZoomToFit->setShortcut( tr( "Ctrl+F" ) );
  connect( actionZoomToFit, SIGNAL( triggered() ), this, SLOT( zoomToFit() ) );

  // ------------ Playing ------------

  actionVideoPlay = new QAction( "Play", this );
  actionVideoPlay->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPlay ) ) );
  connect( actionVideoPlay, SIGNAL( triggered() ), this, SLOT( play() ) );

  actionVideoPause = new QAction( "Pause", this );
  actionVideoPause->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPause ) ) );
  connect( actionVideoPause, SIGNAL( triggered() ), this, SLOT( pause() ) );

  actionVideoStop = new QAction( "Stop", this );
  actionVideoStop->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaStop ) ) );
  connect( actionVideoStop, SIGNAL( triggered() ), this, SLOT( stop() ) );

  mapperSeekVideo = new QSignalMapper( this );
  connect( mapperSeekVideo, SIGNAL( mapped(int) ), this, SLOT( seekEvent(int) ) );

  actionVideoBackward = new QAction( "VideoBackward", this );
  actionVideoBackward->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekBackward ) ) );
  connect( actionVideoBackward, SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( actionVideoBackward, 0 );
  actionVideoForward = new QAction( "VideoForward", this );
  actionVideoForward->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  connect( actionVideoForward, SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( actionVideoForward, 1 );

  actionVideoLoop = new QAction( "Repeat", this );
  actionVideoLoop->setCheckable( true );
  actionVideoLoop->setChecked( false );

  actionVideoLock = new QAction( "VideoLock", this );
  actionVideoLock->setCheckable( true );
  actionVideoLock->setChecked( false );
  connect( actionVideoLock, SIGNAL( triggered() ), this, SLOT( lockButtonEvent() ) );

  actionVideoInterlace = new QAction( "VideoInterlace", this );
  actionVideoCenter = new QAction( "VideoCenter", this );

  m_pcFrameSlider = new QSlider;
  m_pcFrameSlider->setOrientation( Qt::Horizontal );
  m_pcFrameSlider->setMaximumWidth( 100 );
  m_pcFrameSlider->setMaximumWidth( 300 );
  m_pcFrameSlider->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  m_pcFrameSlider->setEnabled( false );
  connect( m_pcFrameSlider, SIGNAL( sliderMoved(int) ), this, SLOT( seekSliderEvent(int) ) );

  // ------------ Window ------------

  actionTile = new QAction( tr( "&Tile" ), this );
  actionTile->setStatusTip( tr( "Tile the windows" ) );
  connect( actionTile, SIGNAL( triggered() ), mdiArea, SLOT( tileSubWindows() ) );

  actionCascade = new QAction( tr( "&Cascade" ), this );
  actionCascade->setStatusTip( tr( "Cascade the windows" ) );
  connect( actionCascade, SIGNAL( triggered() ), mdiArea, SLOT( cascadeSubWindows() ) );

  actionNext = new QAction( tr( "Ne&xt" ), this );
  actionNext->setShortcuts( QKeySequence::NextChild );
  actionNext->setStatusTip( tr( "Move the focus to the next window" ) );
  connect( actionNext, SIGNAL( triggered() ), mdiArea, SLOT( activateNextSubWindow() ) );

  actionPrevious = new QAction( tr( "Pre&vious" ), this );
  actionPrevious->setShortcuts( QKeySequence::PreviousChild );
  actionPrevious->setStatusTip( tr( "Move the focus to the previous window" ) );
  connect( actionPrevious, SIGNAL( triggered() ), mdiArea, SLOT( activatePreviousSubWindow() ) );

  actionSeparator = new QAction( this );
  actionSeparator->setSeparator( true );

  // ------------ About ------------

  actionAbout = new QAction( tr( "&About" ), this );
  actionAbout->setStatusTip( tr( "Show the application's About box" ) );
  connect( actionAbout, SIGNAL( triggered() ), this, SLOT( about() ) );

  actionAboutQt = new QAction( tr( "About &Qt" ), this );
  actionAboutQt->setIcon( QIcon( ":images/qt.png" ) );
  actionAboutQt->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( actionAboutQt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
}

Void plaYUVerApp::createMenus()
{
  m_arrayMenu.resize( TOTAL_MENUS );

  m_arrayMenu[FILE_MENU] = menuBar()->addMenu( tr( "&File" ) );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[OPEN_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[SAVE_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[FORMAT_ACT] );
  m_arrayMenu[FILE_MENU]->addSeparator();
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[CLOSE_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[EXIT_ACT] );

  m_arrayMenu[VIEW_MENU] = menuBar()->addMenu( tr( "&View" ) );
  m_arrayMenu[VIEW_MENU]->addAction( actionZoomIn );
  m_arrayMenu[VIEW_MENU]->addAction( actionZoomOut );
  m_arrayMenu[VIEW_MENU]->addAction( actionNormalSize );
  m_arrayMenu[VIEW_MENU]->addAction( actionZoomToFit );

  m_arrayMenu[VIEW_MENU]->addSeparator();

  // createPopupMenu() Returns a popup menu containing checkable entries for
  // the toolbars and dock widgets present in the main window.
  QMenu *viewsSub = createPopupMenu();
  if( viewsSub )
  {
    actionPopupMenu = m_arrayMenu[VIEW_MENU]->addMenu( viewsSub );
    actionPopupMenu->setText( tr( "&Toolbars/Docks" ) );
  }

  m_arrayMenu[VIDEO_MENU] = menuBar()->addMenu( tr( "Video" ) );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoPlay );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoPause );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoStop );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoBackward );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoForward );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoLoop );
  m_arrayMenu[VIDEO_MENU]->addAction( actionVideoLock );

  QMenu* modules_menu = m_pcModulesHandle->createMenus( menuBar() );
  connect( modules_menu, SIGNAL( triggered(QAction *) ), this, SLOT( selectModule(QAction *) ) );

  m_arrayMenu[WINDOW_MENU] = menuBar()->addMenu( tr( "&Window" ) );
  updateWindowMenu();
  connect( m_arrayMenu[WINDOW_MENU], SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ) );

  menuBar()->addSeparator();

  m_arrayMenu[ABOUT_MENU] = menuBar()->addMenu( tr( "&Help" ) );
  m_arrayMenu[ABOUT_MENU]->addAction( actionAbout );
  m_arrayMenu[ABOUT_MENU]->addAction( actionAboutQt );
}

Void plaYUVerApp::createToolBars()
{
  toolbarFile = addToolBar( tr( "File" ) );
  toolbarFile->addAction( m_arrayActions[OPEN_ACT] );
  toolbarFile->addAction( m_arrayActions[SAVE_ACT] );
  toolbarFile->addAction( m_arrayActions[FORMAT_ACT] );
  toolbarFile->addAction( m_arrayActions[CLOSE_ACT] );

  toolbarView = addToolBar( tr( "Zoom" ) );
  toolbarView->addAction( actionZoomIn );
  toolbarView->addAction( actionZoomOut );
  toolbarView->addAction( actionNormalSize );
  toolbarView->addAction( actionZoomToFit );

  toolbarVideo = addToolBar( tr( "Video" ) );
  toolbarVideo->addAction( actionVideoPlay );
  toolbarVideo->addAction( actionVideoPause );
  toolbarVideo->addAction( actionVideoStop );
  /*
   toolbarVideo->addAction( actionVideoBackward );
   toolbarVideo->addAction( actionVideoForward );
   toolbarVideo->addAction( actionVideoLoop );
   toolbarVideo->addAction( actionVideoLock );
   toolbarVideo->addAction( actionVideoInterlace );
   toolbarVideo->addAction( actionVideoCenter );
   */
  toolbarVideo->addWidget( m_pcFrameSlider );

}

Void plaYUVerApp::createDockWidgets()
{
  // Properties Dock Window
  m_arraySideBars.resize( TOTAL_SIDEBAR );

  m_pcStreamProperties = new StreamPropertiesSideBar( this );
  m_arraySideBars[STREAM_SIDEBAR] = new QDockWidget( tr( "Stream Information" ), this );
  m_arraySideBars[STREAM_SIDEBAR]->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_arraySideBars[STREAM_SIDEBAR]->setWidget( m_pcStreamProperties );
  addDockWidget( Qt::RightDockWidgetArea, m_arraySideBars[STREAM_SIDEBAR] );

  m_pcFrameProperties = new FramePropertiesSideBar( this );
  m_arraySideBars[FRAME_SIDEBAR] = new QDockWidget( tr( "Frame Information" ), this );
  m_arraySideBars[FRAME_SIDEBAR]->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_arraySideBars[FRAME_SIDEBAR]->setWidget( m_pcFrameProperties );
  addDockWidget( Qt::RightDockWidgetArea, m_arraySideBars[FRAME_SIDEBAR] );

}

Void plaYUVerApp::createStatusBar()
{
  statusBar()->showMessage( tr( "Ready" ) );
}

Void plaYUVerApp::readSettings()
{
  PlaYUVerSettings settings;
  QPoint pos = settings.mainWindowPos();
  QSize size = settings.mainWindowSize();
  move( pos );
  resize( size );
  m_cLastOpenPath = settings.lastOpenPath();
}

Void plaYUVerApp::writeSettings()
{
  PlaYUVerSettings settings;
  settings.setMainWindowPos( pos() );
  settings.setMainWindowSize( size() );
  settings.setLastOpenPath( m_cLastOpenPath );
}

}  // NAMESPACE
