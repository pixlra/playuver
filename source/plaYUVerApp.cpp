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
 * \file     plaYUVerApp.cpp
 * \brief    Main definition of the plaYUVerApp app
 */

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QtDebug>

#include "TypeDef.h"
#include "plaYUVerApp.h"
#include "SubWindowHandle.h"
#include "InputStream.h"

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

  mapperWindow = new QSignalMapper( this );
  connect( mapperWindow, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  updateMenus();

  readSettings();

  setWindowTitle( tr( "plaYUVerApp" ) );
  setWindowIcon( QIcon( ":/images/playuver.png" ) );
  setUnifiedTitleAndToolBarOnMac( true );

  playingTimer = new QTimer( this );

  setAcceptDrops( true );
  mdiArea->setAcceptDrops( true );

}

Void plaYUVerApp::closeEvent( QCloseEvent *event )
{
  mdiArea->closeAllSubWindows();
  if( mdiArea->currentSubWindow() )
  {
    event->ignore();
  }
  else
  {
    writeSettings();
    event->accept();
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
  filter << supported << InputStream::supportedReadFormatsList() << tr( "All Files (*)" );

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
      addSubWindow( interfaceChild );
      statusBar()->showMessage( tr( "File loaded" ), 2000 );
      interfaceChild->show();
    }
    else
    {
      interfaceChild->close();
    }
  }
}

void plaYUVerApp::save()
{
  if( activeSubWindow() )
    activeSubWindow()->save();
}

// -----------------------  Playing Functions  --------------------

void plaYUVerApp::play()
{
  if( activeSubWindow() )
  {
    UInt frameRate = activeSubWindow()->getInputStream()->getFrameRate();
    UInt timeInterval = ( UInt )( 1000.0 / frameRate + 0.5 );
    playingTimer->start( timeInterval );
    //connect( playingTimer, SIGNAL( timeout() ), activeSubWindow(), SLOT( playEvent() ) );
    connect( playingTimer, SIGNAL( timeout() ), this, SLOT( playEvent() ) );
  }
}

void plaYUVerApp::pause()
{
  playingTimer->stop();
}

void plaYUVerApp::stop()
{
  if( activeSubWindow() )
  {
    activeSubWindow()->stopEvent();
  }
  playingTimer->stop();
  //disconnect( playingTimer, SIGNAL( timeout() ), 0, 0 );
}

void plaYUVerApp::playEvent()
{
  if( activeSubWindow() )
  {
    if( !activeSubWindow()->playEvent() )
    {
      playingTimer->stop();
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
    activeSubWindow()->scaleView( ( Double )( ratio ) / 100.0 );

    actionZoomIn->setEnabled( activeSubWindow()->getScaleFactor() < 3.0 );
    actionZoomOut->setEnabled( activeSubWindow()->getScaleFactor() > 0.333 );
  }
}

// -----------------------  About Functions  -----------------------

void plaYUVerApp::about()
{
  QMessageBox::about( this, tr( "About plaYUVerApp" ), tr( "The <b>plaYUVerApp</b> is an open-source raw video player " ) );
}

void plaYUVerApp::chageSubWindowSelection()
{
  updateMenus();
  //updateWindowMenu();
}

Void plaYUVerApp::updateMenus()
{
  Bool hasSubWindow = ( activeSubWindow() != 0 );
  actionSave->setEnabled( hasSubWindow );
  actionClose->setEnabled( hasSubWindow );
  actionCloseAll->setEnabled( hasSubWindow );
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
}

void plaYUVerApp::updateWindowMenu()
{
  menuWindow->clear();
  menuWindow->addAction( actionClose );
  menuWindow->addAction( actionCloseAll );
  menuWindow->addSeparator();
  menuWindow->addAction( actionTile );
  menuWindow->addAction( actionCascade );
  menuWindow->addSeparator();
  menuWindow->addAction( actionNext );
  menuWindow->addAction( actionPrevious );
  menuWindow->addAction( actionSeparator );

  //updateMenus();

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
    QAction *action = menuWindow->addAction( text );
    action->setCheckable( true );
    action->setChecked( child == activeSubWindow() );
    connect( action, SIGNAL( triggered() ), mapperWindow, SLOT( map() ) );
    mapperWindow->setMapping( action, windows.at( i ) );
  }
}

SubWindowHandle *plaYUVerApp::createSubWindow()
{
  SubWindowHandle *child = new SubWindowHandle;
  mdiArea->addSubWindow( child );

  return child;
}

void plaYUVerApp::addSubWindow( SubWindowHandle *child )
{
  //connect( child, SIGNAL( areaSelected( QRect ) ), this, SLOT( setSelection( QRect ) ) );

  // Add the interface to the mdiArea
  mdiArea->addSubWindow( child );
  return;
}

Void plaYUVerApp::createActions()
{
  // ------------ File ------------

  actionOpen = new QAction( QIcon( ":/images/open.png" ), tr( "&Open..." ), this );
  actionOpen->setShortcuts( QKeySequence::Open );
  actionOpen->setStatusTip( tr( "Open an existing file" ) );
  connect( actionOpen, SIGNAL( triggered() ), this, SLOT( open() ) );

  actionSave = new QAction( QIcon( ":/images/save.png" ), tr( "&Save..." ), this );
  actionSave->setShortcuts( QKeySequence::SaveAs );
  actionSave->setStatusTip( tr( "Save the document under a new name" ) );
  connect( actionSave, SIGNAL( triggered() ), this, SLOT( save() ) );

  actionExit = new QAction( tr( "E&xit" ), this );
  actionExit->setShortcuts( QKeySequence::Quit );
  actionExit->setStatusTip( tr( "Exit the application" ) );
  connect( actionExit, SIGNAL( triggered() ), qApp, SLOT( closeAllWindows() ) );

  actionClose = new QAction( tr( "Cl&ose" ), this );
  actionClose->setIcon( QIcon( ":/images/close.png" ) );
  actionClose->setStatusTip( tr( "Close the active window" ) );
  connect( actionClose, SIGNAL( triggered() ), mdiArea, SLOT( closeActiveSubWindow() ) );

  actionCloseAll = new QAction( tr( "Close &All" ), this );
  actionCloseAll->setStatusTip( tr( "Close all the windows" ) );
  connect( actionCloseAll, SIGNAL( triggered() ), mdiArea, SLOT( closeAllSubWindows() ) );

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

  actionVideoPlay = new QAction( "VideoPlay", this );
  actionVideoPause = new QAction( "VideoPause", this );
  actionVideoStop = new QAction( "VideoStop", this );
  actionVideoBackward = new QAction( "VideoBackward", this );
  actionVideoForward = new QAction( "VideoForward", this );
  actionVideoLoop = new QAction( "VideoLoop", this );
  actionVideoLock = new QAction( "VideoLock", this );
  actionVideoInterlace = new QAction( "VideoInterlace", this );
  actionVideoCenter = new QAction( "VideoCenter", this );

  actionVideoPlay->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPlay ) ) );
  actionVideoPause->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPause ) ) );
  actionVideoStop->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaStop ) ) );
  actionVideoBackward->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekBackward ) ) );
  actionVideoForward->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  /*
   actionVideoLoop     ->setIcon( QIcon( ":/images/videoloop.png"  ) );
   actionVideoLock     ->setIcon( QIcon( ":/images/lock.png"       ) );
   actionVideoInterlace->setIcon( QIcon( ":/images/interlace.png"  ) );
   actionVideoCenter   ->setIcon( QIcon( ":/images/center.png"     ) );
   */

  connect( actionVideoPlay, SIGNAL( triggered() ), this, SLOT( play() ) );
  connect( actionVideoPause, SIGNAL( triggered() ), this, SLOT( pause() ) );
  connect( actionVideoStop, SIGNAL( triggered() ), this, SLOT( stop() ) );

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
  menuFile = menuBar()->addMenu( tr( "&File" ) );
  menuFile->addAction( actionOpen );
  menuFile->addSeparator();
  menuFile->addAction( actionSave );
  menuFile->addSeparator();
  menuFile->addAction( actionClose );
  menuFile->addAction( actionExit );

  menuView = new QMenu( tr( "&View" ), this );
  menuView->addAction( actionZoomIn );
  menuView->addAction( actionZoomOut );
  menuView->addAction( actionNormalSize );
  menuView->addAction( actionZoomToFit );

  menuWindow = menuBar()->addMenu( tr( "&Window" ) );
  updateWindowMenu();
  connect( menuWindow, SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ) );

  menuBar()->addSeparator();

  menuHelp = menuBar()->addMenu( tr( "&Help" ) );
  menuHelp->addAction( actionAbout );
  menuHelp->addAction( actionAboutQt );
}

Void plaYUVerApp::createToolBars()
{
  toolbarFile = addToolBar( tr( "File" ) );
  toolbarFile->addAction( actionOpen );
  toolbarFile->addAction( actionSave );
  toolbarFile->addAction( actionClose );

  toolbarView = addToolBar( tr( "Zoom" ) );
  toolbarView->addAction( actionZoomIn );
  toolbarView->addAction( actionZoomOut );
  toolbarView->addAction( actionNormalSize );
  toolbarView->addAction( actionZoomToFit );

  toolbarVideo = addToolBar( tr( "Video" ) );
  toolbarVideo->addAction( actionVideoPlay );
  toolbarVideo->addAction( actionVideoPause );
  toolbarVideo->addAction( actionVideoStop );
  toolbarVideo->addAction( actionVideoBackward );
  toolbarVideo->addAction( actionVideoForward );
  toolbarVideo->addAction( actionVideoLoop );
  toolbarVideo->addAction( actionVideoLock );
  toolbarVideo->addAction( actionVideoInterlace );
  toolbarVideo->addAction( actionVideoCenter );

}

Void plaYUVerApp::createStatusBar()
{
  statusBar()->showMessage( tr( "Ready" ) );
}

/*
 * Drag and drop functions
 */
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
        addSubWindow( interfaceChild );
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

Void plaYUVerApp::readSettings()
{
  QSettings settings( "IT", "YUV Player" );
  QPoint pos = settings.value( "pos", QPoint( 200, 200 ) ).toPoint();
  QSize size = settings.value( "size", QSize( 400, 400 ) ).toSize();
  move( pos );
  resize( size );
}

Void plaYUVerApp::writeSettings()
{
  QSettings settings( "IT", "YUV Player" );
  settings.setValue( "pos", pos() );
  settings.setValue( "size", size() );
}

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

}  // NAMESPACE
