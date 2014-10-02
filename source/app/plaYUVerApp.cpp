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
#include "PlaYUVerStream.h"
#include "DialogSubWindowSelector.h"
#ifdef USE_FERVOR
#include "fvupdater.h"
#endif

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QtDebug>

#include "PlaYUVerSettings.h"

#define SYNCHRONISED_ZOON 1

namespace plaYUVer
{

plaYUVerApp::plaYUVerApp()
{

  setWindowModality( Qt::ApplicationModal );

  mdiArea = new PlaYUVerMdiArea;
  setCentralWidget( mdiArea );
  //mdiArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  //mdiArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

  connect( mdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( chageSubWindowSelection() ) );

  mapperWindow = new QSignalMapper( this );
  connect( mapperWindow, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

  m_pcModulesHandle = new ModulesHandle( this, mdiArea );

  createActions();
  createToolBars();
  createDockWidgets();
  createMenus();
  createStatusBar();
  updateMenus();

  readSettings();

  setWindowTitle( QApplication::applicationName() );
  setWindowIcon( QIcon( ":/images/playuver.png" ) );
  setUnifiedTitleAndToolBarOnMac( true );

  m_bIsPlaying = false;
  m_pcPlayingTimer = new QTimer;
#if( QT_VERSION_PLAYUVER == 5 )
  m_pcPlayingTimer->setTimerType( Qt::CoarseTimer );
#endif
  connect( m_pcPlayingTimer, SIGNAL( timeout() ), this, SLOT( playEvent() ) );

  setAcceptDrops( true );
  mdiArea->setAcceptDrops( true );

  m_pcAboutDialog = NULL;
  m_pcCurrentSubWindow = NULL;
}

Void plaYUVerApp::parseArgs( Int argc, Char *argv[] )
{
  if( argc >= 2 )
  {
    for( Int i = 1; i < argc; i++ )
    {
      loadFile( argv[i] );
    }
    mdiArea->tileSubWindows();
    zoomToFitAll();
  }
}

Void plaYUVerApp::about()
{
  if (!m_pcAboutDialog)
    m_pcAboutDialog = new AboutDialog(this);
  m_pcAboutDialog->exec();
//  QString about_message;
//  about_message.append( "The <b>plaYUVer</b> is an open-source raw video player! " );
//  about_message.append( "Developed by " );
//  about_message.append( "João Carreira " );
//  about_message.append( "and " );
//  about_message.append( "Luís Lucas" );
//  QMessageBox::about( this, "About plaYUVer", about_message );
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
      m_pcPlayingTimer->stop();
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
  SubWindowHandle *interfaceChild = plaYUVerApp::findSubWindow( mdiArea, fileName );
  if( interfaceChild )
  {
    mdiArea->setActiveSubWindow( interfaceChild );
    return;
  }

  interfaceChild = new SubWindowHandle( this );  //createSubWindow();
  if( interfaceChild->loadFile( fileName ) )
  {
    statusBar()->showMessage( tr( "Loading file..." ) );
    mdiArea->addSubWindow( interfaceChild );
    interfaceChild->show();
    connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, PlaYUVerFrame *) ), this,
        SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );
    connect( interfaceChild->getViewArea(), SIGNAL( selectionChanged( QRect ) ), this, SLOT( updatePropertiesSelectedArea( QRect ) ) );
    connect( interfaceChild->getViewArea(), SIGNAL( zoomFactorChanged( double , QPoint ) ), this, SLOT( updateZoomFactorSBox() ) );

    interfaceChild->zoomToFit();
    interfaceChild->getViewArea()->setTool( m_appTool );
    updateZoomFactorSBox();
    statusBar()->showMessage( tr( "File loaded" ), 2000 );
  }
  else
  {
    interfaceChild->close();
  }
}

// -----------------------  File Functions  -----------------------

Void plaYUVerApp::open()
{
  QString supported = tr( "Supported Files (" );
  QStringList formatsList;
  QStringList formatsExt = PlaYUVerStream::supportedReadFormatsExt();
  QStringList formatsName = PlaYUVerStream::supportedReadFormatsName();

  for( Int i = 0; i < formatsName.size(); i++ )
  {
    QString currFmt;
    supported.append( " *." );
    supported.append( formatsExt[i] );
    currFmt.append( formatsName[i] );
    currFmt.append( " (*." );
    currFmt.append( formatsExt[i] );
    currFmt.append( ")" );
    formatsList << currFmt;
  }
  supported.append( " )" );

  QStringList filter;
  filter << supported
         << formatsList
         << tr( "All Files (*)" );

  QStringList fileNameList = QFileDialog::getOpenFileNames( this, tr( "Open File" ), m_cLastOpenPath, filter.join( ";;" ) );

  for( Int i = 0; i < fileNameList.size(); i++ )
  {
    if( !fileNameList.at( i ).isEmpty() )
    {
      m_cLastOpenPath = QFileInfo( fileNameList.at( i ) ).path();
      loadFile( fileNameList.at( i ) );
    }
  }
}

Void plaYUVerApp::save()
{
  if( m_pcCurrentSubWindow )
  {
	SubWindowHandle *saveWindow = m_pcCurrentSubWindow;
    QString supported = tr( "Supported Files (" );
    QStringList formatsList;
    QStringList formatsExt = PlaYUVerStream::supportedSaveFormatsExt();
    QStringList formatsName = PlaYUVerStream::supportedSaveFormatsName();

    for( Int i = 0; i < formatsName.size(); i++ )
    {
      QString currFmt;
      supported.append( " *." );
      supported.append( formatsExt[i] );
      currFmt.append( formatsName[i] );
      currFmt.append( " (*." );
      currFmt.append( formatsExt[i] );
      currFmt.append( ")" );
      formatsList << currFmt;
    }
    supported.append( " )" );

    QStringList filter;
    filter << supported
           << formatsList
           << tr( "All Files (*)" );

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Open File" ), QString(), filter.join( ";;" ) );

    if( !fileName.isEmpty() )
    {
      m_cLastOpenPath = QFileInfo( fileName ).path();
      if( !saveWindow->save( fileName ) )
      {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot save file %1" ).arg( fileName ) );
        return;
      }
    }
  }
}

Void plaYUVerApp::format()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->loadFile( m_pcCurrentSubWindow->getCurrentFileName(), true );
    m_pcCurrentSubWindow = NULL;
    chageSubWindowSelection();
  }
}

Void plaYUVerApp::reload()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->reloadFile();
    m_pcCurrentSubWindow = NULL;
    chageSubWindowSelection();
  }
}

Void plaYUVerApp::reloadAll()
{
  SubWindowHandle *subWindow;
  for( Int i = 0; i < mdiArea->subWindowList().size(); i++ )
  {
    subWindow = qobject_cast<SubWindowHandle *>( mdiArea->subWindowList().at( i ) );
    subWindow->reloadFile();
  }
}

Void plaYUVerApp::loadAll()
{
  if( m_pcCurrentSubWindow )
  {
    statusBar()->showMessage( tr( "Loading file into memory ..." ) );
    m_pcCurrentSubWindow->getInputStream()->loadAll();
    statusBar()->showMessage( tr( "File loaded" ), 2000 );
  }
}

Void plaYUVerApp::closeActiveWindow()
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

Void plaYUVerApp::closeAll()
{
  mdiArea->closeAllSubWindows();
}

// -----------------------  Modules Selection  --------------------

Void plaYUVerApp::ModuleHandling( QAction *curr_action )
{
  m_pcFrameProperties->stopHistogram();
  SubWindowHandle *interfaceChild = m_pcModulesHandle->processModuleHandlingOpt();
  if( interfaceChild )
  {
    mdiArea->addSubWindow( interfaceChild );

    interfaceChild->getViewArea()->setTool( m_appTool );
  }
  if( activeSubWindow() )
    m_pcCurrentSubWindow = activeSubWindow();
  updateFrameProperties();
  return;
}

// -----------------------  Update Properties   --------------------

Void plaYUVerApp::updateProperties()
{
  updateStreamProperties();
  updateFrameProperties();
}

Void plaYUVerApp::updateStreamProperties()
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

Void plaYUVerApp::updatePropertiesSelectedArea( QRect area )
{
  if( m_pcCurrentSubWindow )
  {
    if( area.isValid() )
    {
      m_pcFrameProperties->setSelection( area );
    }
  }
  else
  {
    m_pcFrameProperties->setData( NULL );
  }
}

Void plaYUVerApp::updateZoomFactorSBox()
{
  if( m_pcCurrentSubWindow && m_pcCurrentSubWindow->getViewArea() )
  {
    Double factor = m_pcCurrentSubWindow->getViewArea()->getZoomFactor();
    m_pcZoomFactorSBox->setValue( factor * 100 );
  }
  else
  {
    m_pcZoomFactorSBox->setValue( 0.0 );
  }
}

// -----------------------  Playing Functions  --------------------

UInt64 plaYUVerApp::getMaxFrameNumber()
{
  UInt currFrames;
  UInt64 maxFrames = INT_MAX;
  if( m_pcCurrentSubWindow )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
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
      maxFrames = m_pcCurrentSubWindow->getInputStream()->getFrameNum();
    }
  }
  return maxFrames;
}
Void plaYUVerApp::setTimerStatus()
{
  Bool status = false;
  for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
  {
    status |= m_acPlayingSubWindows.at( i )->isPlaying();
  }
  if( status )
  {
    if( !m_bIsPlaying )
    {
      m_cTimer.start();
      m_bIsPlaying = true;
    }
  }
  else
  {
    m_pcPlayingTimer->stop();
    m_bIsPlaying = false;
  }
}

Void plaYUVerApp::startPlay()
{
  if( !m_bIsPlaying )
  {
    UInt frameRate = m_acPlayingSubWindows.at( 0 )->getInputStream()->getFrameRate();
    UInt timeInterval = ( UInt )( 1000.0 / frameRate + 0.5 );
    qDebug( ) << "Desired frame rate: "
              << QString::number( 1000 / timeInterval )
              << " fps";
    m_pcPlayingTimer->start( timeInterval );
    m_cTimer.start();
    m_bIsPlaying = true;
  }
}

Void plaYUVerApp::play()
{
  if( !m_pcCurrentSubWindow )
    return;

  if( !m_pcCurrentSubWindow->getInputStream() )
    return;

  if( !m_pcCurrentSubWindow->isPlaying() )
  {
    if( m_arrayActions[VIDEO_LOCK_ACT]->isChecked() && m_acPlayingSubWindows.size() )
    {
      if( !m_pcCurrentSubWindow->play() )
      {
        return;
      }
      if( !m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
      {
        m_acPlayingSubWindows.append( m_pcCurrentSubWindow );
        m_pcCurrentSubWindow->seekAbsoluteEvent( m_acPlayingSubWindows.at( 0 )->getInputStream()->getCurrFrameNum() );
        updateTotalFrameNum();
      }
    }
    else
    {
      m_pcPlayingTimer->stop();
      m_acPlayingSubWindows.clear();
      if( !m_pcCurrentSubWindow->play() )
      {
        return;
      }
      m_acPlayingSubWindows.append( m_pcCurrentSubWindow );
      m_uiAveragePlayInterval = 0;
      m_bIsPlaying = false;
    }
    startPlay();
    m_arrayActions[PLAY_ACT]->setIcon( style()->standardIcon( QStyle::SP_MediaPause ) );
  }
  else
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
    updateFrameProperties();
    m_arrayActions[PLAY_ACT]->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
  }
}

Void plaYUVerApp::pause()
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
  updateFrameProperties();
}

Void plaYUVerApp::stop()
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
    updateFrameProperties();
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
  updateTotalFrameNum();
}

Void plaYUVerApp::playEvent()
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
      }
      else
      {
        for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
        {
          m_acPlayingSubWindows.at( i )->seekAbsoluteEvent( 0 );
        }
      }
      break;
    case -3:
      m_acPlayingSubWindows.at( i )->close();
      m_acPlayingSubWindows.remove( i );
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
  m_pcQualityMeasurement->updateSidebarData();
  m_uiAveragePlayInterval = ( m_uiAveragePlayInterval + time ) / 2;
}

Void plaYUVerApp::updateCurrFrameNum()
{
  if( m_pcCurrentSubWindow && m_pcCurrentSubWindow->getInputStream() )
  {
    Int frame_num = m_pcCurrentSubWindow->getInputStream()->getCurrFrameNum();
    m_pcFrameSlider->setValue( frame_num );
    m_pcCurrFrameNumLabel->setText( QString( tr( "%1" ) ).arg( frame_num + 1 ) );
  }
  else
  {
    m_pcCurrFrameNumLabel->setText( "1" );
  }
}
Void plaYUVerApp::updateTotalFrameNum()
{
  UInt64 total_frame_num = 1;
  if( m_pcCurrentSubWindow && m_pcCurrentSubWindow->getInputStream() )
  {
    total_frame_num = getMaxFrameNumber();
  }
  m_pcFrameSlider->setMaximum( total_frame_num - 1 );
  m_pcTotalFrameNumLabel->setText( QString( tr( "%1" ) ).arg( total_frame_num ) );
}

Void plaYUVerApp::seekEvent( Int direction )
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
    m_pcQualityMeasurement->updateSidebarData();
    updateFrameProperties();
    updateCurrFrameNum();
  }
}

Void plaYUVerApp::seekSliderEvent( Int new_frame_num )
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
    m_pcQualityMeasurement->updateSidebarData();
    updateFrameProperties();
    updateCurrFrameNum();
  }
}

Void plaYUVerApp::lockButtonEvent()
{
  if( !m_arrayActions[VIDEO_LOCK_ACT]->isChecked() )
  {
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      m_acPlayingSubWindows.clear();
      m_acPlayingSubWindows.append( m_pcCurrentSubWindow );
    }
    else
    {
      if( m_acPlayingSubWindows.size() > 1 )
        m_acPlayingSubWindows.clear();
    }
  }
}

Void plaYUVerApp::videoSelectionButtonEvent()
{
  DialogSubWindowSelector dialogWindowsSelection( this, mdiArea );
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
    SubWindowHandle *subWindow;
    QString windowName;
    Double scaleFactor = qobject_cast<SubWindowHandle *>( mdiArea->subWindowList().at( 0 ) )->getZoomFactor();
    for( Int i = 0; i < mdiArea->subWindowList().size(); i++ )
    {
      subWindow = qobject_cast<SubWindowHandle *>( mdiArea->subWindowList().at( i ) );
      windowName = subWindow->getWindowName();
      if( selectedWindows.contains( windowName ) )
      {
        m_acPlayingSubWindows.append( subWindow );
        subWindow->seekAbsoluteEvent( m_acPlayingSubWindows.at( 0 )->getInputStream()->getCurrFrameNum() );
        subWindow->scaleView( scaleFactor );
        subWindow->play();
        m_pcFrameSlider->setMaximum( qMin( m_pcFrameSlider->maximum(), ( Int )subWindow->getInputStream()->getFrameNum() - 1 ) );
      }
    }
    m_arrayActions[VIDEO_LOCK_ACT]->setChecked( true );
    //startPlay();
    return;
  }
}

// -----------------------  Zoom Functions  -----------------------

Void plaYUVerApp::normalSize()
{
  if( m_pcCurrentSubWindow )
  {
#if SYNCHRONISED_ZOON
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->normalSize();
      }
    }
    else
#endif
    {
      m_pcCurrentSubWindow->normalSize();
    }
    updateZoomFactorSBox();
  }
}

Void plaYUVerApp::zoomToFit()
{
  if( m_pcCurrentSubWindow )
  {
#if SYNCHRONISED_ZOON
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->zoomToFit();
      }
    }
    else
#endif
    {
      m_pcCurrentSubWindow->zoomToFit();
    }
    updateZoomFactorSBox();
  }
}

Void plaYUVerApp::zoomToFitAll()
{
  SubWindowHandle *subWindow;
  for( Int i = 0; i < mdiArea->subWindowList().size(); i++ )
  {
    subWindow = qobject_cast<SubWindowHandle *>( mdiArea->subWindowList().at( i ) );
    subWindow->zoomToFit();
  }
  updateZoomFactorSBox();
}

Void plaYUVerApp::scaleFrame( int ratio )
{
  if( m_pcCurrentSubWindow )
  {
#if SYNCHRONISED_ZOON
    if( m_acPlayingSubWindows.contains( m_pcCurrentSubWindow ) )
    {
      for( Int i = 0; i < m_acPlayingSubWindows.size(); i++ )
      {
        m_acPlayingSubWindows.at( i )->scaleViewByRatio( ( Double )( ratio ) / 100.0 );
      }
    }
    else
#endif
    {
      m_pcCurrentSubWindow->scaleViewByRatio( ( Double )( ratio ) / 100.0 );
    }
    updateZoomFactorSBox();
//    m_arrayActions[ZOOM_IN_ACT]->setEnabled( activeSubWindow()->getScaleFactor() < 3.0 );
//    m_arrayActions[ZOOM_OUT_ACT]->setEnabled( activeSubWindow()->getScaleFactor() > 0.333 );
  }
}

Void plaYUVerApp::setZoomFromSBox( double zoom )
{
  Double lastZoom;
  if( activeSubWindow() )
  {
    lastZoom = activeSubWindow()->getViewArea()->getZoomFactor() * 100;
    if( zoom / lastZoom != 1.0 )
      activeSubWindow()->scaleViewByRatio( zoom / lastZoom );
  }
}

Void plaYUVerApp::chageSubWindowSelection()
{
  SubWindowHandle *new_window = activeSubWindow();
  if( activeSubWindow() != m_pcCurrentSubWindow )
  {
    if( activeSubWindow() )
    {
      m_pcCurrentSubWindow = new_window;
      if( !plaYUVerApp::findSubWindow( mdiArea, m_pcCurrentSubWindow->getRefSubWindow() ) )
      {
        m_pcCurrentSubWindow->setRefSubWindow( NULL );
      }
      updateCurrFrameNum();
      updateTotalFrameNum();
      updateStreamProperties();
      updateFrameProperties();
      updateZoomFactorSBox();
    }
    m_pcCurrentSubWindow = new_window;
  }
  updateStreamProperties();
  updateFrameProperties();
  m_pcQualityMeasurement->updateCurrentWindow( m_pcCurrentSubWindow );
  m_pcQualityMeasurement->updateSubWindowList();
  updateMenus();
}

// -----------------------  Status bar Functions  -----------------------

Void plaYUVerApp::updatePixelValueStatusBar( const QPoint & pos, PlaYUVerFrame* frame )
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
    statusBar()->showMessage( strStatus, 3000 );
  }
  else
  {
    statusBar()->showMessage( " " );
  }
}

// -----------------------  Drag and drop functions  ----------------------

Void plaYUVerApp::dragEnterEvent( QDragEnterEvent *event )
{
  //setText(tr("<drop content>"));
  setBackgroundRole( QPalette::Highlight );
  event->acceptProposedAction();
}

Void plaYUVerApp::dropEvent( QDropEvent *event )
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

SubWindowHandle* plaYUVerApp::findSubWindow( const QMdiArea* mdiArea, const QString& fileName )
{
  QString canonicalFilePath = QFileInfo( fileName ).canonicalFilePath();

  foreach( QMdiSubWindow * window, mdiArea->subWindowList() ){
  SubWindowHandle *mdiChild = qobject_cast<SubWindowHandle *>( window);
  if( mdiChild->getCurrentFileName() == canonicalFilePath )
  return mdiChild;
}
  return 0;
}

SubWindowHandle* plaYUVerApp::findSubWindow( const QMdiArea* mdiArea, const SubWindowHandle* subWindow )
{
  foreach( QMdiSubWindow * window, mdiArea->subWindowList() ){
  SubWindowHandle *mdiChild = qobject_cast<SubWindowHandle *>( window);
  if( mdiChild == subWindow )
  return mdiChild;
}
return 0;
}

Void plaYUVerApp::setNavigationTool()
{
  m_appTool = NavigationTool;
  setAllSubWindowTool();
}

Void plaYUVerApp::setSelectionTool()
{
  m_appTool = NormalSelectionTool;
  setAllSubWindowTool();
}

Void plaYUVerApp::setAllSubWindowTool()
{
  foreach( QMdiSubWindow * window, mdiArea->subWindowList() ){
  SubWindowHandle *mdiChild = qobject_cast<SubWindowHandle *>( window);
  mdiChild->getViewArea()->setTool( m_appTool );
}
}

Void plaYUVerApp::setActiveSubWindow( QWidget *window )
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
  m_pcZoomFactorSBox->setEnabled( hasSubWindow );

  m_arrayActions[PLAY_ACT]->setEnabled( hasSubWindow );
  //m_arrayActions[PAUSE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[STOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_FORWARD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOOP_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOCK_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[VIDEO_LOCK_SELECTION_ACT]->setEnabled( hasSubWindow );
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

Void plaYUVerApp::updateWindowMenu()
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
      text = tr( "&%1 %2" ).arg( i + 1 ).arg( child->getWindowName() );
    }
    else
    {
      text = tr( "%1 %2" ).arg( i + 1 ).arg( child->getWindowName() );
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
  m_arrayActions[OPEN_ACT] = new QAction( QIcon( ":/images/open.png" ), tr( "&Open" ), this );
  m_arrayActions[OPEN_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogOpenButton ) );
  m_arrayActions[OPEN_ACT]->setShortcuts( QKeySequence::Open );
  m_arrayActions[OPEN_ACT]->setStatusTip( tr( "Open stream" ) );
  connect( m_arrayActions[OPEN_ACT], SIGNAL( triggered() ), this, SLOT( open() ) );

  m_arrayActions[SAVE_ACT] = new QAction( QIcon( ":/images/save.png" ), tr( "&Save Frame" ), this );
  m_arrayActions[SAVE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogSaveButton ) );
  m_arrayActions[SAVE_ACT]->setShortcuts( QKeySequence::SaveAs );
  m_arrayActions[SAVE_ACT]->setStatusTip( tr( "Save current frame" ) );
  connect( m_arrayActions[SAVE_ACT], SIGNAL( triggered() ), this, SLOT( save() ) );

  m_arrayActions[FORMAT_ACT] = new QAction( tr( "&Format" ), this );
  m_arrayActions[FORMAT_ACT]->setIcon( QIcon( ":/images/configuredialog.png" ) );
  m_arrayActions[FORMAT_ACT]->setShortcut( Qt::CTRL + Qt::Key_F );
  m_arrayActions[FORMAT_ACT]->setStatusTip( tr( "Open format dialog" ) );
  connect( m_arrayActions[FORMAT_ACT], SIGNAL( triggered() ), this, SLOT( format() ) );

  m_arrayActions[RELOAD_ACT] = new QAction( tr( "&Reload" ), this );
  m_arrayActions[RELOAD_ACT]->setIcon( style()->standardIcon( QStyle::SP_BrowserReload ) );
  m_arrayActions[RELOAD_ACT]->setShortcut( Qt::CTRL + Qt::Key_R );
  m_arrayActions[RELOAD_ACT]->setStatusTip( tr( "Reload current sequence" ) );
  connect( m_arrayActions[RELOAD_ACT], SIGNAL( triggered() ), this, SLOT( reload() ) );

  m_arrayActions[RELOAD_ALL_ACT] = new QAction( tr( "Reload All" ), this );
  //m_arrayActions[RELOAD_ACT]->setIcon( QIcon( ":/images/configuredialog.png" ) );
  m_arrayActions[RELOAD_ALL_ACT]->setStatusTip( tr( "Reload all sequences" ) );
  connect( m_arrayActions[RELOAD_ALL_ACT], SIGNAL( triggered() ), this, SLOT( reloadAll() ) );

  m_arrayActions[LOAD_ALL_ACT] = new QAction( tr( "Preload" ), this );
  //m_arrayActions[LOAD_ALL_ACT]->setIcon( QIcon( ":/images/configuredialog.png" ) );
  m_arrayActions[LOAD_ALL_ACT]->setStatusTip( tr( "Load sequence into memory (caution)" ) );
  connect( m_arrayActions[LOAD_ALL_ACT], SIGNAL( triggered() ), this, SLOT( loadAll() ) );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "&Close" ), this );
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

  m_arrayActions[ZOOM_IN_ACT] = new QAction( tr( "Zoom &In (+25%)" ), this );
  m_arrayActions[ZOOM_IN_ACT]->setIcon( QIcon( ":/images/zoomin.png" ) );
  m_arrayActions[ZOOM_IN_ACT]->setShortcut( tr( "Ctrl++" ) );
  m_arrayActions[ZOOM_IN_ACT]->setStatusTip( tr( "Scale the image up by 25%" ) );
  connect( m_arrayActions[ZOOM_IN_ACT], SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( m_arrayActions[ZOOM_IN_ACT], 125 );

  m_arrayActions[ZOOM_OUT_ACT] = new QAction( tr( "Zoom &Out (-25%)" ), this );
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
  //m_arrayActions[ZOOM_FIT_ACT]->setShortcut( tr( "Ctrl+F" ) );
  connect( m_arrayActions[ZOOM_FIT_ACT], SIGNAL( triggered() ), this, SLOT( zoomToFit() ) );

  // ------------ Playing ------------

  m_arrayActions[PLAY_ACT] = new QAction( "Play", this );
  m_arrayActions[PLAY_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPlay ) ) );
  m_arrayActions[PLAY_ACT]->setShortcut( Qt::Key_Space );
  connect( m_arrayActions[PLAY_ACT], SIGNAL( triggered() ), this, SLOT( play() ) );

//  m_arrayActions[PAUSE_ACT] = new QAction( "Pause", this );
//  m_arrayActions[PAUSE_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaPause ) ) );
//  connect( m_arrayActions[PAUSE_ACT], SIGNAL( triggered() ), this, SLOT( pause() ) );

  m_arrayActions[STOP_ACT] = new QAction( "Stop", this );
  m_arrayActions[STOP_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaStop ) ) );
  connect( m_arrayActions[STOP_ACT], SIGNAL( triggered() ), this, SLOT( stop() ) );

  mapperSeekVideo = new QSignalMapper( this );
  connect( mapperSeekVideo, SIGNAL( mapped(int) ), this, SLOT( seekEvent(int) ) );

  m_arrayActions[VIDEO_BACKWARD_ACT] = new QAction( "Video Backward", this );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekBackward ) ) );
  m_arrayActions[VIDEO_BACKWARD_ACT]->setShortcut( Qt::Key_Left );
  connect( m_arrayActions[VIDEO_BACKWARD_ACT], SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( m_arrayActions[VIDEO_BACKWARD_ACT], 0 );

  m_arrayActions[VIDEO_FORWARD_ACT] = new QAction( "Video Forward", this );
  m_arrayActions[VIDEO_FORWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  m_arrayActions[VIDEO_FORWARD_ACT]->setShortcut( Qt::Key_Right );
  connect( m_arrayActions[VIDEO_FORWARD_ACT], SIGNAL( triggered() ), mapperSeekVideo, SLOT( map() ) );
  mapperSeekVideo->setMapping( m_arrayActions[VIDEO_FORWARD_ACT], 1 );

  m_arrayActions[VIDEO_LOOP_ACT] = new QAction( "Repeat", this );
  m_arrayActions[VIDEO_LOOP_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_LOOP_ACT]->setChecked( false );

  m_arrayActions[VIDEO_LOCK_ACT] = new QAction( "Video Lock", this );
  m_arrayActions[VIDEO_LOCK_ACT]->setCheckable( true );
  m_arrayActions[VIDEO_LOCK_ACT]->setChecked( false );
  connect( m_arrayActions[VIDEO_LOCK_ACT], SIGNAL( triggered() ), this, SLOT( lockButtonEvent() ) );

  m_arrayActions[VIDEO_LOCK_SELECTION_ACT] = new QAction( "Video Lock Window Selection", this );
  m_arrayActions[VIDEO_FORWARD_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_MediaSeekForward ) ) );
  connect( m_arrayActions[VIDEO_LOCK_SELECTION_ACT], SIGNAL( triggered() ), this, SLOT( videoSelectionButtonEvent() ) );

  m_pcFrameSlider = new QSlider;
  m_pcFrameSlider->setOrientation( Qt::Horizontal );
  m_pcFrameSlider->setMaximumWidth( 100 );
  m_pcFrameSlider->setMaximumWidth( /* 300 */2000 );
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

  m_arrayActions[TILE_WINDOWS_ACT] = new QAction( tr( "Tile" ), this );
  m_arrayActions[TILE_WINDOWS_ACT]->setIcon( QIcon( ":images/windowstile.png" ) );
  m_arrayActions[TILE_WINDOWS_ACT]->setStatusTip( tr( "Tile the windows" ) );
  connect( m_arrayActions[TILE_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( tileSubWindows() ) );

  m_arrayActions[CASCADE_WINDOWS_ACT] = new QAction( tr( "Cascade" ), this );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setIcon( QIcon( ":images/windowscascade.png" ) );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setStatusTip( tr( "Cascade the windows" ) );
  connect( m_arrayActions[CASCADE_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( cascadeSubWindows() ) );

  m_arrayActions[NEXT_WINDOWS_ACT] = new QAction( tr( "Ne&xt" ), this );
  m_arrayActions[NEXT_WINDOWS_ACT]->setShortcuts( QKeySequence::NextChild );
  m_arrayActions[NEXT_WINDOWS_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_ArrowRight ) ) );
  m_arrayActions[NEXT_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the next window" ) );
  connect( m_arrayActions[NEXT_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( activateNextSubWindow() ) );

  m_arrayActions[PREVIOUS_WINDOWS_ACT] = new QAction( tr( "Pre&vious" ), this );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setShortcuts( QKeySequence::PreviousChild );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_ArrowLeft ) ) );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the previous window" ) );
  connect( m_arrayActions[PREVIOUS_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( activatePreviousSubWindow() ) );

  m_arrayActions[SEPARATOR_ACT] = new QAction( this );
  m_arrayActions[SEPARATOR_ACT]->setSeparator( true );

  // ------------ About ------------

#ifdef USE_FERVOR
  m_arrayActions[UPDATE_ACT] = new QAction( tr( "&Update" ), this );
  m_arrayActions[UPDATE_ACT]->setStatusTip( tr( "Check for updates" ) );
  connect( m_arrayActions[UPDATE_ACT], SIGNAL( triggered() ), FvUpdater::sharedUpdater(), SLOT( CheckForUpdatesNotSilent() ) );
#endif

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
  m_arrayMenu[FILE_MENU]->addSeparator();
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[FORMAT_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[RELOAD_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[RELOAD_ALL_ACT] );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[LOAD_ALL_ACT] );
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
  m_arrayMenu[DOCK_VIEW_MENU] = createPopupMenu();
  if( m_arrayMenu[DOCK_VIEW_MENU] )
  {
    actionPopupMenu = m_arrayMenu[VIEW_MENU]->addMenu( m_arrayMenu[DOCK_VIEW_MENU] );
    actionPopupMenu->setText( tr( "&Toolbars/Docks" ) );
  }

  m_arrayMenu[TOOLS_MENU] = menuBar()->addMenu( tr( "Tools" ) );
  m_arrayMenu[TOOLS_MENU]->addAction( m_arrayActions[NAVIGATION_TOOL_ACT] );
  m_arrayMenu[TOOLS_MENU]->addAction( m_arrayActions[SELECTION_TOOL_ACT] );

  m_arrayMenu[VIDEO_MENU] = menuBar()->addMenu( tr( "Video" ) );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[PLAY_ACT] );
  //m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[PAUSE_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[STOP_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_LOOP_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_LOCK_ACT] );
  m_arrayMenu[VIDEO_MENU]->addAction( m_arrayActions[VIDEO_LOCK_SELECTION_ACT] );

  QMenu* modules_menu = m_pcModulesHandle->createMenus( menuBar() );
  connect( modules_menu, SIGNAL( triggered(QAction *) ), this, SLOT( ModuleHandling(QAction *) ) );

  m_arrayMenu[WINDOW_MENU] = menuBar()->addMenu( tr( "&Window" ) );
  updateWindowMenu();
  connect( m_arrayMenu[WINDOW_MENU], SIGNAL( aboutToShow() ), this, SLOT( updateWindowMenu() ) );

  menuBar()->addSeparator();

  m_arrayMenu[ABOUT_MENU] = menuBar()->addMenu( tr( "&Help" ) );
#ifdef USE_FERVOR
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[UPDATE_ACT] );
#endif
  m_arrayMenu[ABOUT_MENU]->addSeparator();
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUT_ACT] );
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUTQT_ACT] );

}

Void plaYUVerApp::createToolBars()
{
  m_arrayToolBars.resize( TOTAL_TOOLBAR );

  m_arrayToolBars[FILE_TOOLBAR] = new QToolBar( tr( "File" ) );
  m_arrayToolBars[FILE_TOOLBAR]->addAction( m_arrayActions[OPEN_ACT] );
  m_arrayToolBars[FILE_TOOLBAR]->addAction( m_arrayActions[SAVE_ACT] );
  m_arrayToolBars[FILE_TOOLBAR]->addAction( m_arrayActions[FORMAT_ACT] );
  m_arrayToolBars[FILE_TOOLBAR]->addAction( m_arrayActions[RELOAD_ACT] );
  m_arrayToolBars[FILE_TOOLBAR]->addAction( m_arrayActions[CLOSE_ACT] );

  m_arrayToolBars[FILE_TOOLBAR]->setMovable( false );
  addToolBar( Qt::TopToolBarArea, m_arrayToolBars[FILE_TOOLBAR] );

  m_arrayToolBars[VIEW_TOOLBAR] = new QToolBar( tr( "View" ) );
  m_pcZoomFactorSBox = new QDoubleSpinBox;
  m_pcZoomFactorSBox->setRange( 1.0, 10000.0 );
  m_pcZoomFactorSBox->setSingleStep( 10.0 );
  m_pcZoomFactorSBox->setValue( 100.0 );
  m_pcZoomFactorSBox->setSuffix( "%" );
  connect( m_pcZoomFactorSBox, SIGNAL( valueChanged(double) ), this, SLOT( setZoomFromSBox(double) ) );
  m_arrayToolBars[VIEW_TOOLBAR]->addWidget( m_pcZoomFactorSBox );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_IN_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_OUT_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_NORMAL_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_FIT_ACT] );

  m_arrayToolBars[FILE_TOOLBAR]->setMovable( false );
  addToolBar( Qt::TopToolBarArea, m_arrayToolBars[VIEW_TOOLBAR] );

  m_arrayToolBars[VIDEO_TOOLBAR] = new QToolBar( tr( "Video" ) );
  m_arrayToolBars[VIDEO_TOOLBAR]->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
  m_arrayToolBars[VIDEO_TOOLBAR]->addAction( m_arrayActions[PLAY_ACT] );
  //m_arrayToolBars[VIDEO_TOOLBAR]->addAction( m_arrayActions[PAUSE_ACT] );
  m_arrayToolBars[VIDEO_TOOLBAR]->addAction( m_arrayActions[STOP_ACT] );
  m_arrayToolBars[VIDEO_TOOLBAR]->addAction( m_arrayActions[VIDEO_BACKWARD_ACT] );
  m_arrayToolBars[VIDEO_TOOLBAR]->addWidget( m_pcFrameSlider );
  m_arrayToolBars[VIDEO_TOOLBAR]->addAction( m_arrayActions[VIDEO_FORWARD_ACT] );
  m_arrayToolBars[VIDEO_TOOLBAR]->addWidget( new QLabel );
  m_pcCurrFrameNumLabel = new QLabel;
  m_pcCurrFrameNumLabel->setText( "-" );
  m_arrayToolBars[VIDEO_TOOLBAR]->addWidget( m_pcCurrFrameNumLabel );
  QLabel *forwardslash = new QLabel;
  forwardslash->setText( "/" );
  m_arrayToolBars[VIDEO_TOOLBAR]->addWidget( forwardslash );
  m_pcTotalFrameNumLabel = new QLabel;
  m_pcTotalFrameNumLabel->setText( "-" );
  m_arrayToolBars[VIDEO_TOOLBAR]->addWidget( m_pcTotalFrameNumLabel );

  addToolBar( Qt::TopToolBarArea, m_arrayToolBars[VIDEO_TOOLBAR] );
}

Void plaYUVerApp::createDockWidgets()
{
  // Properties Dock Window
  m_arraySideBars.resize( TOTAL_DOCK );

  m_pcStreamProperties = new StreamPropertiesSideBar( this );
  m_arraySideBars[STREAM_DOCK] = new QDockWidget( tr( "Stream Information" ), this );
  m_arraySideBars[STREAM_DOCK]->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_arraySideBars[STREAM_DOCK]->setWidget( m_pcStreamProperties );
  addDockWidget( Qt::RightDockWidgetArea, m_arraySideBars[STREAM_DOCK] );
  connect( m_arraySideBars[STREAM_DOCK], SIGNAL( visibilityChanged(bool) ), this, SLOT( updateProperties() ) );

  m_pcFrameProperties = new FramePropertiesSideBar( this, &m_bIsPlaying );
  m_arraySideBars[FRAME_DOCK] = new QDockWidget( tr( "Frame Information" ), this );
  m_arraySideBars[FRAME_DOCK]->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_arraySideBars[FRAME_DOCK]->setWidget( m_pcFrameProperties );
  addDockWidget( Qt::RightDockWidgetArea, m_arraySideBars[FRAME_DOCK] );
  connect( m_arraySideBars[FRAME_DOCK], SIGNAL( visibilityChanged(bool) ), this, SLOT( updateProperties() ) );

  QMainWindow::tabifyDockWidget( m_arraySideBars[FRAME_DOCK], m_arraySideBars[STREAM_DOCK] );
  //QMainWindow::setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);

  m_pcQualityMeasurement = new QualityMeasurementSidebar( this, mdiArea );
  m_arraySideBars[QUALITY_DOCK] = new QDockWidget( tr( "Quality Measurement" ), this );
  m_arraySideBars[QUALITY_DOCK]->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_arraySideBars[QUALITY_DOCK]->setWidget( m_pcQualityMeasurement );
  addDockWidget( Qt::RightDockWidgetArea, m_arraySideBars[QUALITY_DOCK] );

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
  m_appTool = ( enum eTool )settings.getSelectedTool();
  setAllSubWindowTool();
  m_arrayActions[VIDEO_LOOP_ACT]->setChecked( settings.getRepeat() );
  m_arrayActions[VIDEO_LOCK_ACT]->setChecked( settings.getVideoLock() );
  Bool visibleStreamProp, visibleFrameProp, visibleQualityMeasure;
  settings.getDockVisibility( visibleStreamProp, visibleFrameProp, visibleQualityMeasure );
  if( !visibleStreamProp )
    m_arraySideBars[STREAM_DOCK]->close();
  if( !visibleFrameProp )
    m_arraySideBars[FRAME_DOCK]->close();
  if( !visibleQualityMeasure )
    m_arraySideBars[QUALITY_DOCK]->close();

}

Void plaYUVerApp::writeSettings()
{
  PlaYUVerSettings settings;
  settings.setMainWindowPos( pos() );
  settings.setMainWindowSize( size() );
  settings.setLastOpenPath( m_cLastOpenPath );
  settings.setSelectedTool( ( Int )m_appTool );
  settings.setDockVisibility( m_pcStreamProperties->isVisible(), m_pcFrameProperties->isVisible(), m_pcQualityMeasurement->isVisible() );
  settings.setPlayingSettings( m_arrayActions[VIDEO_LOOP_ACT]->isChecked(), m_arrayActions[VIDEO_LOCK_ACT]->isChecked() );
}

}  // NAMESPACE
