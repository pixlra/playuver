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

Void plaYUVerApp::parseArgs( Int argc, Char *argv[] )
{
  if( argc == 2 )
  {
    loadFile( argv[1] );
  }
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

Void plaYUVerApp::loadFile( QString fileName )
{
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
    connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, PlaYUVerFrame *) ), this,
        SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );
//    connect( interfaceChild->getViewArea(), SIGNAL( selectionChanged( QRect ) ), this,
//            SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );

    interfaceChild->zoomToFit();
    interfaceChild->getViewArea()->setTool( m_appTool );
  }
  else
  {
    interfaceChild->close();
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
    loadFile( fileName );
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
  SubWindowHandle *interfaceChild = m_pcModulesHandle->toggleSelectedModuleIf( m_pcCurrentSubWindow );
  if( interfaceChild )
  {
    mdiArea->addSubWindow( interfaceChild );
    interfaceChild->show();
    interfaceChild->zoomToFit();
    interfaceChild->getViewArea()->setTool( m_appTool );
  }
  updateFrameProperties();
  return;
}

// -----------------------  Update Properties   --------------------

void plaYUVerApp::updateStreamProperties()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcStreamProperties->setData( m_pcCurrentSubWindow->getInputStream() );
  }
  else
  {
    m_pcStreamProperties->setData( NULL );
  }
}

Void plaYUVerApp::updateFrameProperties()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcFrameProperties->setData( m_pcCurrentSubWindow->getCurrFrame() );
    //m_pcFrameProperties->setSelection( m_pcCurrentSubWindow->getViewArea()->selectedArea() );
  }
  else
  {
    m_pcFrameProperties->setData( NULL );
  }
}

void plaYUVerApp::updatePropertiesSelectedArea( QRect area )
{
  if( m_pcCurrentSubWindow )
  {
    if( area.isNull() )
    {
      m_pcFrameProperties->setData( m_pcCurrentSubWindow->getInputStream()->getCurrFrame() );
      m_pcFrameProperties->setSelection( area );
    }
    else
    {
      m_pcFrameProperties->setSelection( area );
    }
  }
  else
  {
    m_pcFrameProperties->setData( NULL );
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
  if( !m_pcCurrentSubWindow )
    return;

  if( !m_pcCurrentSubWindow->getInputStream() )
    return;

  if( m_arrayActions[VIDEO_LOCK_ACT]->isChecked() && m_acPlayingSubWindows.size() )
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
    updateCurrFrameNum();
  }
  setTimerStatus();
}

void plaYUVerApp::stop()
{
  if( m_acPlayingSubWindows.size() > 0 )
  {
    for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
    {
      m_acPlayingSubWindows.at( i )->stop();
    }
    m_acPlayingSubWindows.clear();
    setTimerStatus();
    updateCurrFrameNum();
    if( m_uiAveragePlayInterval )
      qDebug( ) << "Real display time: "
                << QString::number( 1000 / m_uiAveragePlayInterval )
                << " fps";
    m_uiAveragePlayInterval = 0;
  }
  else
  {
    if( m_pcCurrentSubWindow )
    {
      seekSliderEvent( 0 );
    }
  }
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
      break;
    case -2:
      if( !m_arrayActions[VIDEO_LOOP_ACT]->isChecked() )
      {
        stop();
        updateCurrFrameNum();
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
  updateCurrFrameNum();
  updateFrameProperties();
  m_uiAveragePlayInterval = ( m_uiAveragePlayInterval + time ) / 2;
}

Void plaYUVerApp::updateCurrFrameNum()
{
  if( m_pcCurrentSubWindow )
  {
    Int frame_num = m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum();
    m_pcFrameSlider->setValue( frame_num );
    m_pcCurrFrameNumLabel->setText( QString( tr( "%1" ) ).arg( frame_num + 1 ) );
  }
}
Void plaYUVerApp::updateTotalFrameNum( UInt total_frame_num )
{
  if( m_pcCurrentSubWindow && total_frame_num == 0 )
  {
    total_frame_num = m_pcCurrentSubWindow->getInputStream()->getFrameNum();
  }
  m_pcFrameSlider->setMaximum( total_frame_num - 1 );
  m_pcTotalFrameNumLabel->setText( QString( tr( "%1" ) ).arg( total_frame_num ) );
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
    }
    updateFrameProperties();
    updateCurrFrameNum();
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
    }
    updateFrameProperties();
    updateCurrFrameNum();
  }
}

void plaYUVerApp::lockButtonEvent()
{
  if( !m_arrayActions[VIDEO_LOCK_ACT]->isChecked() )
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
    activeSubWindow()->scaleViewByRatio( ( Double )( ratio ) / 100.0 );

//    m_arrayActions[ZOOM_IN_ACT]->setEnabled( activeSubWindow()->getScaleFactor() < 3.0 );
//    m_arrayActions[ZOOM_OUT_ACT]->setEnabled( activeSubWindow()->getScaleFactor() > 0.333 );
  }
}

// -----------------------  About Functions  -----------------------

void plaYUVerApp::about()
{
  QString about_message;
  about_message.append("The <b>plaYUVerApp</b> is an open-source raw video player");
//  about_message.append("\n\r");
//  about_message.append("João Carreira");
//  about_message.append("\n\r");
//  about_message.append("Luís Lucas");
  QMessageBox::about( this, tr( "About plaYUVerApp" ), about_message );
}

void plaYUVerApp::chageSubWindowSelection()
{
  SubWindowHandle *new_window = activeSubWindow();
  if( activeSubWindow() != m_pcCurrentSubWindow )
  {
    if( activeSubWindow() )
    {
      m_pcCurrentSubWindow = new_window;
      updateCurrFrameNum();
      updateTotalFrameNum();
      updateStreamProperties();
      updateFrameProperties();
    }
    m_pcCurrentSubWindow = new_window;
  }
  updateStreamProperties();
  updateFrameProperties();
  updateMenus();
}

// -----------------------  Status bar Functions  -----------------------

void plaYUVerApp::updatePixelValueStatusBar( const QPoint & pos, PlaYUVerFrame* frame )
{
  Pixel sPixelValue;
  Int iWidth, iHeight;
  Int posX = pos.x(), posY = pos.y();
  QString strPixel;
  QString strStatus = QString( "(%1,%2)   " ).arg( posX ).arg( posY );
  PlaYUVerFrame *curFrame = frame;

  iWidth = frame->getWidth();
  iHeight = frame->getHeight();

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
    loadFile( fileName );
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

void plaYUVerApp::setNavigationTool()
{
  m_appTool = NavigationTool;
  setAllSubWindowTool();
}

void plaYUVerApp::setSelectionTool()
{
  m_appTool = NormalSelectionTool;
  setAllSubWindowTool();
}

void plaYUVerApp::setAllSubWindowTool()
{
  foreach( QMdiSubWindow * window, mdiArea->subWindowList() ){
  SubWindowHandle *mdiChild = qobject_cast<SubWindowHandle *>( window);
  mdiChild->getViewArea()->setTool( m_appTool );
}
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
  m_arrayActions[TILE_WINDOWS_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[SEPARATOR_ACT]->setVisible( hasSubWindow );

  m_arrayActions[ZOOM_IN_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_OUT_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_NORMAL_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_FIT_ACT]->setEnabled( hasSubWindow );

  m_arrayActions[PLAY_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[PAUSE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[STOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_FORWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOCK_ACT]->setEnabled( hasSubWindow );
  m_pcFrameSlider->setEnabled( hasSubWindow );
  if( !hasSubWindow )
  {
    m_pcFrameSlider->setValue( 0 );
    m_pcCurrFrameNumLabel->setText( "-" );
    m_pcTotalFrameNumLabel->setText( "-" );
  }
  m_arrayActions[NAVIGATION_TOOL_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[SELECTION_TOOL_ACT]->setEnabled( hasSubWindow );

  m_pcModulesHandle->updateMenus( hasSubWindow );
}

void plaYUVerApp::updateWindowMenu()
{
  m_arrayMenu[WINDOW_MENU]->clear();
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[CLOSE_ACT] );
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[CLOSEALL_ACT] );
  m_arrayMenu[WINDOW_MENU]->addSeparator();
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[TILE_WINDOWS_ACT] );
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[CASCADE_WINDOWS_ACT] );
  m_arrayMenu[WINDOW_MENU]->addSeparator();
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[NEXT_WINDOWS_ACT] );
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[PREVIOUS_WINDOWS_ACT] );
  m_arrayMenu[WINDOW_MENU]->addAction( m_arrayActions[SEPARATOR_ACT] );

  updateMenus();

  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  m_arrayActions[SEPARATOR_ACT]->setVisible( !windows.isEmpty() );
  Int number_windows = windows.size();

  if( number_windows > 1 )
  {
    m_arrayActions[NEXT_WINDOWS_ACT]->setEnabled( true );
    m_arrayActions[PREVIOUS_WINDOWS_ACT]->setEnabled( true );
  }
  else
  {
    m_arrayActions[NEXT_WINDOWS_ACT]->setEnabled( false );
    m_arrayActions[PREVIOUS_WINDOWS_ACT]->setEnabled( false );
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
  m_arrayActions[OPEN_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogOpenButton ) );
  m_arrayActions[OPEN_ACT]->setShortcuts( QKeySequence::Open );
  m_arrayActions[OPEN_ACT]->setStatusTip( tr( "Open stream" ) );
  connect( m_arrayActions[OPEN_ACT], SIGNAL( triggered() ), this, SLOT( open() ) );

  m_arrayActions[SAVE_ACT] = new QAction( QIcon( ":/images/save.png" ), tr( "&Save..." ), this );
  m_arrayActions[SAVE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogSaveButton ) );
  m_arrayActions[SAVE_ACT]->setShortcuts( QKeySequence::SaveAs );
  m_arrayActions[SAVE_ACT]->setStatusTip( tr( "Save current frame" ) );
  connect( m_arrayActions[SAVE_ACT], SIGNAL( triggered() ), this, SLOT( save() ) );

  m_arrayActions[FORMAT_ACT] = new QAction( tr( "&Format" ), this );
  m_arrayActions[FORMAT_ACT]->setIcon( QIcon( ":/images/configuredialog.png" ) );
  m_arrayActions[FORMAT_ACT]->setStatusTip( tr( "Open format dialog" ) );
  connect( m_arrayActions[FORMAT_ACT], SIGNAL( triggered() ), this, SLOT( format() ) );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "Cl&ose" ), this );
  // m_arrayActions[CLOSE_ACT]->setIcon( QIcon( ":/images/close.png" ) );
  m_arrayActions[CLOSE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogCloseButton ) );

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

  m_arrayActions[ZOOM_IN_ACT] = new QAction( tr( "Zoom &In (25%)" ), this );
  m_arrayActions[ZOOM_IN_ACT]->setIcon( QIcon( ":/images/zoomin.png" ) );
  m_arrayActions[ZOOM_IN_ACT]->setShortcut( tr( "Ctrl++" ) );
  m_arrayActions[ZOOM_IN_ACT]->setStatusTip( tr( "Scale the image up by 25%" ) );
  connect( m_arrayActions[ZOOM_IN_ACT], SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( m_arrayActions[ZOOM_IN_ACT], 125 );

  m_arrayActions[ZOOM_OUT_ACT] = new QAction( tr( "Zoom &Out (25%)" ), this );
  m_arrayActions[ZOOM_OUT_ACT]->setIcon( QIcon( ":/images/zoomout.png" ) );
  m_arrayActions[ZOOM_OUT_ACT]->setShortcut( tr( "Ctrl+-" ) );
  m_arrayActions[ZOOM_OUT_ACT]->setStatusTip( tr( "Scale the image down by 25%" ) );
  connect( m_arrayActions[ZOOM_OUT_ACT], SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( m_arrayActions[ZOOM_OUT_ACT], 80 );

  m_arrayActions[ZOOM_NORMAL_ACT] = new QAction( tr( "&Normal Size" ), this );
  m_arrayActions[ZOOM_NORMAL_ACT]->setIcon( QIcon( ":/images/zoomtonormal.png" ) );
  m_arrayActions[ZOOM_NORMAL_ACT]->setShortcut( tr( "Ctrl+N" ) );
  m_arrayActions[ZOOM_NORMAL_ACT]->setStatusTip( tr( "Show the image at its original size" ) );
  connect( m_arrayActions[ZOOM_NORMAL_ACT], SIGNAL( triggered() ), this, SLOT( normalSize() ) );

  m_arrayActions[ZOOM_FIT_ACT] = new QAction( tr( "Zoom to &Fit" ), this );
  m_arrayActions[ZOOM_FIT_ACT]->setIcon( QIcon( ":/images/fittowindow.png" ) );
  m_arrayActions[ZOOM_FIT_ACT]->setStatusTip( tr( "Zoom in or out to fit on the window." ) );
  m_arrayActions[ZOOM_FIT_ACT]->setShortcut( tr( "Ctrl+F" ) );
  connect( m_arrayActions[ZOOM_FIT_ACT], SIGNAL( triggered() ), this, SLOT( zoomToFit() ) );

  // ------------ Playing ------------

  m_arrayActions[PLAY_ACT] = new QAction( "Play", this );
  m_arrayActions[PLAY_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPlay ) ) );
  connect( m_arrayActions[PLAY_ACT], SIGNAL( triggered() ), this, SLOT( play() ) );

  m_arrayActions[PAUSE_ACT] = new QAction( "Pause", this );
  m_arrayActions[PAUSE_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPause ) ) );
  connect( m_arrayActions[PAUSE_ACT], SIGNAL( triggered() ), this, SLOT( pause() ) );

  m_arrayActions[STOP_ACT] = new QAction( "Stop", this );
  m_arrayActions[STOP_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaStop ) ) );
  connect( m_arrayActions[STOP_ACT], SIGNAL( triggered() ), this, SLOT( stop() ) );

  mapperSeekVideo = new QSignalMapper( this );
  connect( mapperSeekVideo, SIGNAL( mapped(int) ), this, SLOT( seekEvent(int) ) );

  m_arrayActions[VIDEO_BACKWARD_ACT] = new QAction( "VideoBackward", this );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekBackward ) ) );
  connect( m_arrayActions[VIDEO_BACKWARD_ACT], SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( m_arrayActions[VIDEO_BACKWARD_ACT], 0 );
  m_arrayActions[VIDEO_FORWARD_ACT] = new QAction( "VideoForward", this );
  m_arrayActions[VIDEO_FORWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  connect( m_arrayActions[VIDEO_FORWARD_ACT], SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( m_arrayActions[VIDEO_FORWARD_ACT], 1 );

  m_arrayActions[VIDEO_LOOP_ACT] = new QAction( "Repeat", this );
  m_arrayActions[VIDEO_LOOP_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_LOOP_ACT]->setChecked( false );

  m_arrayActions[VIDEO_LOCK_ACT] = new QAction( "VideoLock", this );
  m_arrayActions[VIDEO_LOCK_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_LOCK_ACT]->setChecked( false );
  connect( m_arrayActions[VIDEO_LOCK_ACT], SIGNAL( triggered() ), this, SLOT( lockButtonEvent() ) );

  m_pcFrameSlider = new QSlider;
  m_pcFrameSlider->setOrientation( Qt::Horizontal );
  m_pcFrameSlider->setMaximumWidth( 100 );
  m_pcFrameSlider->setMaximumWidth( 300 );
  m_pcFrameSlider->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  m_pcFrameSlider->setEnabled( false );
  connect( m_pcFrameSlider, SIGNAL( sliderMoved(int) ), this, SLOT( seekSliderEvent(int) ) );

  // ------------ Tools ------------
  actionGroupTools = new QActionGroup( this );
  actionGroupTools->setExclusive( true );

  m_appTool = NavigationTool;

  m_arrayActions[NAVIGATION_TOOL_ACT] = new QAction( tr( "&Navigation Tool" ), this );
  m_arrayActions[NAVIGATION_TOOL_ACT]->setCheckable( true );
  m_arrayActions[NAVIGATION_TOOL_ACT]->setChecked( true );
  actionGroupTools->addAction( m_arrayActions[NAVIGATION_TOOL_ACT] );
  connect( m_arrayActions[NAVIGATION_TOOL_ACT], SIGNAL( triggered() ), this, SLOT( setNavigationTool() ) );

  m_arrayActions[SELECTION_TOOL_ACT] = new QAction( "&Selection Tool", this );
  m_arrayActions[SELECTION_TOOL_ACT]->setCheckable( true );
  m_arrayActions[SELECTION_TOOL_ACT]->setChecked( false );
  actionGroupTools->addAction( m_arrayActions[SELECTION_TOOL_ACT] );
  connect( m_arrayActions[SELECTION_TOOL_ACT], SIGNAL( triggered() ), this, SLOT( setSelectionTool() ) );

  // ------------ Window ------------

  m_arrayActions[TILE_WINDOWS_ACT] = new QAction( tr( "&Tile" ), this );
  m_arrayActions[TILE_WINDOWS_ACT]->setStatusTip( tr( "Tile the windows" ) );
  connect( m_arrayActions[TILE_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( tileSubWindows() ) );

  m_arrayActions[CASCADE_WINDOWS_ACT] = new QAction( tr( "&Cascade" ), this );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setStatusTip( tr( "Cascade the windows" ) );
  connect( m_arrayActions[CASCADE_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( cascadeSubWindows() ) );

  m_arrayActions[NEXT_WINDOWS_ACT] = new QAction( tr( "Ne&xt" ), this );
  m_arrayActions[NEXT_WINDOWS_ACT]->setShortcuts( QKeySequence::NextChild );
  m_arrayActions[NEXT_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the next window" ) );
  connect( m_arrayActions[NEXT_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( activateNextSubWindow() ) );

  m_arrayActions[PREVIOUS_WINDOWS_ACT] = new QAction( tr( "Pre&vious" ), this );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setShortcuts( QKeySequence::PreviousChild );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the previous window" ) );
  connect( m_arrayActions[PREVIOUS_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( activatePreviousSubWindow() ) );

  m_arrayActions[SEPARATOR_ACT] = new QAction( this );
  m_arrayActions[SEPARATOR_ACT]->setSeparator( true );

  // ------------ About ------------

  m_arrayActions[ABOUT_ACT] = new QAction( tr( "&About" ), this );
  m_arrayActions[ABOUT_ACT]->setStatusTip( tr( "Show the application's About box" ) );
  connect( m_arrayActions[ABOUT_ACT], SIGNAL( triggered() ), this, SLOT( about() ) );

  m_arrayActions[ABOUTQT_ACT] = new QAction( tr( "About &Qt" ), this );
  m_arrayActions[ABOUTQT_ACT]->setIcon( QIcon( ":images/qt.png" ) );
  m_arrayActions[ABOUTQT_ACT]->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( m_arrayActions[ABOUTQT_ACT], SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
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
  m_arrayMenu[VIEW_MENU]->addAction( m_arrayActions[ZOOM_IN_ACT] );
  m_arrayMenu[VIEW_MENU]->addAction( m_arrayActions[ZOOM_OUT_ACT] );
  m_arrayMenu[VIEW_MENU]->addAction( m_arrayActions[ZOOM_NORMAL_ACT] );
  m_arrayMenu[VIEW_MENU]->addAction( m_arrayActions[ZOOM_FIT_ACT] );

  m_arrayMenu[VIEW_MENU]->addSeparator();

  // createPopupMenu() Returns a popup menu containing checkable entries for
  // the toolbars and dock widgets present in the main window.
  QMenu *viewsSub = createPopupMenu();
  if( viewsSub )
  {
    actionPopupMenu = m_arrayMenu[VIEW_MENU]->addMenu( viewsSub );
    actionPopupMenu->setText( tr( "&Toolbars/Docks" ) );
  }

  m_arrayMenu[TOOLS_MENU] = menuBar()->addMenu( tr( "Tools" ) );
  m_arrayMenu[TOOLS_MENU]->addAction( m_arrayActions[NAVIGATION_TOOL_ACT] );
  m_arrayMenu[TOOLS_MENU]->addAction( m_arrayActions[SELECTION_TOOL_ACT] );

  m_arrayMenu[VIDEO_MENU] = menuBar()->addMenu( tr( "Video" ) );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[PLAY_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[PAUSE_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[STOP_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_LOOP_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_LOCK_ACT] );

  QMenu* modules_menu = m_pcModulesHandle->createMenus( menuBar() );
  connect( modules_menu, SIGNAL( triggered(QAction *) ), this, SLOT( selectModule(QAction *) ) );

  m_arrayMenu[WINDOW_MENU] = menuBar()->addMenu( tr( "&Window" ) );
  updateWindowMenu();
  connect( m_arrayMenu[WINDOW_MENU], SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ) );

  menuBar()->addSeparator();

  m_arrayMenu[ABOUT_MENU] = menuBar()->addMenu( tr( "&Help" ) );
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUT_ACT] );
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUTQT_ACT] );
}

Void plaYUVerApp::createToolBars()
{
  toolbarFile = addToolBar( tr( "File" ) );
  toolbarFile->addAction( m_arrayActions[OPEN_ACT] );
  toolbarFile->addAction( m_arrayActions[SAVE_ACT] );
  toolbarFile->addAction( m_arrayActions[FORMAT_ACT] );
  toolbarFile->addAction( m_arrayActions[CLOSE_ACT] );

  toolbarView = addToolBar( tr( "Zoom" ) );
  toolbarView->addAction( m_arrayActions[ZOOM_IN_ACT] );
  toolbarView->addAction( m_arrayActions[ZOOM_OUT_ACT] );
  toolbarView->addAction( m_arrayActions[ZOOM_NORMAL_ACT] );
  toolbarView->addAction( m_arrayActions[ZOOM_FIT_ACT] );

  toolbarVideo = addToolBar( tr( "Video" ) );
  toolbarVideo->addAction( m_arrayActions[PLAY_ACT] );
  toolbarVideo->addAction( m_arrayActions[PAUSE_ACT] );
  toolbarVideo->addAction( m_arrayActions[STOP_ACT] );
  toolbarVideo->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  toolbarVideo->addWidget( m_pcFrameSlider );
  toolbarVideo->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );

  toolbarVideo->addWidget( new QLabel );
  m_pcCurrFrameNumLabel = new QLabel;
  m_pcCurrFrameNumLabel->setText( "-" );
  toolbarVideo->addWidget( m_pcCurrFrameNumLabel );
  QLabel *forwardslash = new QLabel;
  forwardslash->setText( "/" );
  toolbarVideo->addWidget( forwardslash );
  m_pcTotalFrameNumLabel = new QLabel;
  m_pcTotalFrameNumLabel->setText( "-" );
  toolbarVideo->addWidget( m_pcTotalFrameNumLabel );
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
