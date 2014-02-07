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

  connect( mdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( updateMenus() ) );
  windowMapper = new QSignalMapper( this );
  connect( windowMapper, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

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

  connect( playingTimer, SIGNAL( timeout() ), this, SLOT( playEvent() ) );

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
  UInt frameRate;
  UInt timeInterval;

  frameRate = activeSubWindow()->getInputStream()->getFrameRate();
  timeInterval = ( UInt )( 1000.0 / frameRate + 0.5 );

  playingTimer->start( timeInterval );

}

void plaYUVerApp::pause()
{
  playingTimer->stop();
}

void plaYUVerApp::stop()
{
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

void plaYUVerApp::zoomIn()
{
  scaleImage( 1.25 );
}

void plaYUVerApp::zoomOut()
{
  scaleImage( 0.8 );
}

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

void plaYUVerApp::scaleImage( double factor )
{

  if( activeSubWindow() )
  {
    activeSubWindow()->scaleView( factor );

    zoomInAct->setEnabled( activeSubWindow()->getScaleFactor() < 3.0 );
    zoomOutAct->setEnabled( activeSubWindow()->getScaleFactor() > 0.333 );
  }
}

// -----------------------  About Functions  -----------------------

void plaYUVerApp::about()
{
  QMessageBox::about( this, tr( "About plaYUVerApp" ), tr( "The <b>plaYUVerApp</b> is an open-source raw video player " ) );
}

void plaYUVerApp::updateMenus()
{
  Bool hasSubWindow = ( activeSubWindow() != 0 );
  saveAct->setEnabled( hasSubWindow );
  closeAct->setEnabled( hasSubWindow );
  closeAllAct->setEnabled( hasSubWindow );
  tileAct->setEnabled( hasSubWindow );
  cascadeAct->setEnabled( hasSubWindow );
  nextAct->setEnabled( hasSubWindow );
  previousAct->setEnabled( hasSubWindow );
  separatorAct->setVisible( hasSubWindow );

  zoomInAct->setEnabled( hasSubWindow );
  zoomOutAct->setEnabled( hasSubWindow );
  normalSizeAct->setEnabled( hasSubWindow );
  zoomToFitAct->setEnabled( hasSubWindow );

  actionVideoPlay->setEnabled( hasSubWindow );
  actionVideoPause->setEnabled( hasSubWindow );
  actionVideoStop->setEnabled( hasSubWindow );
  actionVideoBackward->setEnabled( hasSubWindow );
  actionVideoForward->setEnabled( hasSubWindow );
}

void plaYUVerApp::updateWindowMenu()
{
  windowMenu->clear();
  windowMenu->addAction( closeAct );
  windowMenu->addAction( closeAllAct );
  windowMenu->addSeparator();
  windowMenu->addAction( tileAct );
  windowMenu->addAction( cascadeAct );
  windowMenu->addSeparator();
  windowMenu->addAction( nextAct );
  windowMenu->addAction( previousAct );
  windowMenu->addAction( separatorAct );

  updateMenus();

  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible( !windows.isEmpty() );

  for( Int i = 0; i < windows.size(); ++i )
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
    QAction *action = windowMenu->addAction( text );
    action->setCheckable( true );
    action->setChecked( child == activeSubWindow() );
    connect( action, SIGNAL( triggered() ), windowMapper, SLOT( map() ) );
    windowMapper->setMapping( action, windows.at( i ) );
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

  openAct = new QAction( QIcon( ":/images/open.png" ), tr( "&Open..." ), this );
  openAct->setShortcuts( QKeySequence::Open );
  openAct->setStatusTip( tr( "Open an existing file" ) );
  connect( openAct, SIGNAL( triggered() ), this, SLOT( open() ) );

  saveAct = new QAction( QIcon( ":/images/save.png" ), tr( "&Save..." ), this );
  saveAct->setShortcuts( QKeySequence::SaveAs );
  saveAct->setStatusTip( tr( "Save the document under a new name" ) );
  connect( saveAct, SIGNAL( triggered() ), this, SLOT( save() ) );

  exitAct = new QAction( tr( "E&xit" ), this );
  exitAct->setShortcuts( QKeySequence::Quit );
  exitAct->setStatusTip( tr( "Exit the application" ) );
  connect( exitAct, SIGNAL( triggered() ), qApp, SLOT( closeAllWindows() ) );

  closeAct = new QAction( tr( "Cl&ose" ), this );
  closeAct->setIcon( QIcon( ":/images/close.png" ) );
  closeAct->setStatusTip( tr( "Close the active window" ) );
  connect( closeAct, SIGNAL( triggered() ), mdiArea, SLOT( closeActiveSubWindow() ) );

  closeAllAct = new QAction( tr( "Close &All" ), this );
  closeAllAct->setStatusTip( tr( "Close all the windows" ) );
  connect( closeAllAct, SIGNAL( triggered() ), mdiArea, SLOT( closeAllSubWindows() ) );

  // ------------ View ------------

  zoomInAct = new QAction( tr( "Zoom &In (25%)" ), this );
  zoomInAct->setIcon( QIcon( ":/images/zoomin.png" ) );
  zoomInAct->setShortcut( tr( "Ctrl++" ) );
  zoomInAct->setStatusTip( tr( "Scale the image up by 25%" ) );
  connect( zoomInAct, SIGNAL( triggered() ), this, SLOT( zoomIn() ) );

  zoomOutAct = new QAction( tr( "Zoom &Out (25%)" ), this );
  zoomOutAct->setIcon( QIcon( ":/images/zoomout.png" ) );
  zoomOutAct->setShortcut( tr( "Ctrl+-" ) );
  zoomOutAct->setStatusTip( tr( "Scale the image down by 25%" ) );
  connect( zoomOutAct, SIGNAL( triggered() ), this, SLOT( zoomOut() ) );

  normalSizeAct = new QAction( tr( "&Normal Size" ), this );
  normalSizeAct->setIcon( QIcon( ":/images/zoomtonormal.png" ) );
  normalSizeAct->setShortcut( tr( "Ctrl+N" ) );
  normalSizeAct->setStatusTip( tr( "Show the image at its original size" ) );
  connect( normalSizeAct, SIGNAL( triggered() ), this, SLOT( normalSize() ) );

  zoomToFitAct = new QAction( tr( "Zoom to &Fit" ), this );
  zoomToFitAct->setIcon( QIcon( ":/images/fittowindow.png" ) );
  zoomToFitAct->setStatusTip( tr( "Zoom in or out to fit on the window." ) );
  zoomToFitAct->setShortcut( tr( "Ctrl+F" ) );
  connect( zoomToFitAct, SIGNAL( triggered() ), this, SLOT( zoomToFit() ) );

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

  tileAct = new QAction( tr( "&Tile" ), this );
  tileAct->setStatusTip( tr( "Tile the windows" ) );
  connect( tileAct, SIGNAL( triggered() ), mdiArea, SLOT( tileSubWindows() ) );

  cascadeAct = new QAction( tr( "&Cascade" ), this );
  cascadeAct->setStatusTip( tr( "Cascade the windows" ) );
  connect( cascadeAct, SIGNAL( triggered() ), mdiArea, SLOT( cascadeSubWindows() ) );

  nextAct = new QAction( tr( "Ne&xt" ), this );
  nextAct->setShortcuts( QKeySequence::NextChild );
  nextAct->setStatusTip( tr( "Move the focus to the next window" ) );
  connect( nextAct, SIGNAL( triggered() ), mdiArea, SLOT( activateNextSubWindow() ) );

  previousAct = new QAction( tr( "Pre&vious" ), this );
  previousAct->setShortcuts( QKeySequence::PreviousChild );
  previousAct->setStatusTip( tr( "Move the focus to the previous window" ) );
  connect( previousAct, SIGNAL( triggered() ), mdiArea, SLOT( activatePreviousSubWindow() ) );

  separatorAct = new QAction( this );
  separatorAct->setSeparator( true );

  // ------------ About ------------

  aboutAct = new QAction( tr( "&About" ), this );
  aboutAct->setStatusTip( tr( "Show the application's About box" ) );
  connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );

  aboutQtAct = new QAction( tr( "About &Qt" ), this );
  aboutQtAct->setIcon( QIcon( ":images/qt.png" ) );
  aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
}

Void plaYUVerApp::createMenus()
{
  fileMenu = menuBar()->addMenu( tr( "&File" ) );
  fileMenu->addAction( openAct );
  fileMenu->addSeparator();
  fileMenu->addAction( saveAct );
  fileMenu->addSeparator();
  fileMenu->addAction( closeAct );
  fileMenu->addAction( exitAct );

  viewMenu = new QMenu( tr( "&View" ), this );
  viewMenu->addAction( zoomInAct );
  viewMenu->addAction( zoomOutAct );
  viewMenu->addAction( normalSizeAct );
  viewMenu->addAction( zoomToFitAct );

  windowMenu = menuBar()->addMenu( tr( "&Window" ) );
  updateWindowMenu();
  connect( windowMenu, SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ) );

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu( tr( "&Help" ) );
  helpMenu->addAction( aboutAct );
  helpMenu->addAction( aboutQtAct );
}

Void plaYUVerApp::createToolBars()
{
  fileToolBar = addToolBar( tr( "File" ) );
  fileToolBar->addAction( openAct );
  fileToolBar->addAction( saveAct );
  fileToolBar->addAction( closeAct );

  viewToolBar = addToolBar( tr( "Zoom" ) );
  viewToolBar->addAction( zoomInAct );
  viewToolBar->addAction( zoomOutAct );
  viewToolBar->addAction( normalSizeAct );
  viewToolBar->addAction( zoomToFitAct );

  videoToolBar = addToolBar( tr( "Video" ) );
  videoToolBar->addAction( actionVideoPlay );
  videoToolBar->addAction( actionVideoPause );
  videoToolBar->addAction( actionVideoStop );
  videoToolBar->addAction( actionVideoBackward );
  videoToolBar->addAction( actionVideoForward );
  videoToolBar->addAction( actionVideoLoop );
  videoToolBar->addAction( actionVideoLock );
  videoToolBar->addAction( actionVideoInterlace );
  videoToolBar->addAction( actionVideoCenter );
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
