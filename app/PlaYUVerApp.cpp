/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerApp.cpp
 * \brief    Main definition of the PlaYUVerApp app
 */

#include <QtDebug>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDoubleSpinBox>
#include <QActionGroup>
#include <QAction>
#include <QSignalMapper>
#include "PlaYUVerApp.h"
#ifdef USE_FERVOR
#include "fvupdater.h"
#endif
#include "lib/PlaYUVerOptions.h"
#include "PlaYUVerSubWindowHandle.h"
#include "AboutDialog.h"
#include "SubWindowAbstract.h"
#include "VideoSubWindow.h"
#include "VideoHandle.h"
#include "QualityHandle.h"
#include "ModulesHandle.h"
#include "DialogSubWindowSelector.h"

#define SYNCHRONISED_ZOON 1

PlaYUVerApp::PlaYUVerApp() : m_pcCurrentSubWindow( NULL ), m_pcCurrentVideoSubWindow( NULL ), m_pcAboutDialog( NULL )
{
  setWindowModality( Qt::ApplicationModal );
  setWindowModality( Qt::NonModal );

// DBus
#ifdef USE_QTDBUS
  m_pDBusAdaptor = new PlaYUVerAppAdaptor( this );
#endif

  m_pcWindowHandle = new PlaYUVerSubWindowHandle( this );

  m_appModuleVideo = new VideoHandle( this, m_pcWindowHandle );
  m_appModuleQuality = new QualityHandle( this, m_pcWindowHandle );
  m_appModuleExtensions = new ModulesHandle( this, m_pcWindowHandle, m_appModuleVideo );

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
  setCentralWidget( m_pcWindowHandle );
  setAcceptDrops( true );
  setBackgroundRole( QPalette::Background );

  connect( m_pcWindowHandle, SIGNAL( windowActivated() ), this, SLOT( update() ) );
  connect( m_pcWindowHandle, SIGNAL( changed() ), this, SLOT( update() ) );
  connect( m_appModuleVideo, SIGNAL( changed() ), this, SLOT( update() ) );
  connect( m_appModuleQuality, SIGNAL( changed() ), this, SLOT( update() ) );
  connect( m_appModuleExtensions, SIGNAL( changed() ), this, SLOT( update() ) );
}

Bool PlaYUVerApp::parseArgs( Int argc, Char* argv[] )
{
  Bool bRet = false;
  PlaYUVerOptions pcCmdParser;

  std::vector<String> m_apcInputs;
  String strResolution( "" );
  String strPelFmt( "" );

  pcCmdParser.addOptions()                      /**/
      ( "input,i", m_apcInputs, "input file" )  /**/
      ( "size,s", strResolution, "size (WxH)" ) /**/
      ( "pel_fmt", strPelFmt, "pixel format" );

  if( !pcCmdParser.parse( argc, argv ) )
  {
    bRet |= true;
  }

  if( pcCmdParser.hasOpt( "help" ) )
  {
    printf( "Usage: %s [options] input_file[s]\n", argv[0] );
    pcCmdParser.doHelp( std::cout );
    bRet |= true;
  }
  if( pcCmdParser.hasOpt( "input" ) )
  {
    for( UInt i = 1; i < m_apcInputs.size(); i++ )
    {
      loadFile( QString::fromStdString( m_apcInputs[i] ) );
    }
  }
  std::list<const Char*>& argv_unhandled = pcCmdParser.getUnhandledArgs();
  for( std::list<const Char*>::const_iterator it = argv_unhandled.begin(); it != argv_unhandled.end(); it++ )
  {
    loadFile( QString::fromStdString( *it ) );
  }
  m_pcWindowHandle->tileSubWindows();
  zoomToFitAll();
  return bRet;
}

Void PlaYUVerApp::about()
{
  if( !m_pcAboutDialog )
    m_pcAboutDialog = new AboutDialog( this );
  m_pcAboutDialog->exec();
}

Void PlaYUVerApp::closeEvent( QCloseEvent* event )
{
  Int mayCloseAll = true;
  Int msgBoxCloseRet = QMessageBox::Yes;

  QList<SubWindowAbstract*> subWindowList = m_pcWindowHandle->findSubWindow();
  if( subWindowList.size() >= 1 )
  {
    QMessageBox msgBoxClose( QMessageBox::Question, "PlaYUVer", "There are open files!",
                             QMessageBox::Yes | QMessageBox::No, this );
    msgBoxClose.setDefaultButton( QMessageBox::No );
    msgBoxClose.setInformativeText( "Close all?" );
    msgBoxCloseRet = msgBoxClose.exec();
  }
  switch( msgBoxCloseRet )
  {
    case QMessageBox::Yes:
      mayCloseAll = true;
      for( Int i = 0; i < subWindowList.size(); i++ )
      {
        mayCloseAll &= subWindowList.at( i )->mayClose();
      }

      if( mayCloseAll )
      {
        // m_pcPlayingTimer->stop();
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

Void PlaYUVerApp::closeAll()
{
  m_pcWindowHandle->removeAllSubWindow();
}

Void PlaYUVerApp::loadFile( QString fileName, PlaYUVerStreamInfo* pStreamInfo )
{
  if( !QFileInfo( fileName ).exists() )
  {
    printMessage( "File " + fileName + " do not exist!", LOG_ERROR );
    return;
  }
  VideoSubWindow* videoSubWindow = PlaYUVerApp::findVideoStreamSubWindow( m_pcWindowHandle, fileName );
  if( videoSubWindow )
  {
    m_pcWindowHandle->setActiveSubWindow( videoSubWindow );
    return;
  }
  videoSubWindow = new VideoSubWindow( VideoSubWindow::VIDEO_STREAM_SUBWINDOW );  // createSubWindow();
  SubWindowAbstract* subWindow = videoSubWindow;
  if( !pStreamInfo )
  {
    Int idx = findPlaYUVerStreamInfo( m_aRecentFileStreamInfo, fileName );
    pStreamInfo = (PlaYUVerStreamInfo*)( idx >= 0 ? &m_aRecentFileStreamInfo.at( idx ) : NULL );
  }

  try
  {
    Bool opened = false;

    if( !pStreamInfo )
      opened = videoSubWindow->loadFile( fileName );
    else
      opened = videoSubWindow->loadFile( pStreamInfo );

    if( opened )
    {
      printMessage( "Loading file...", LOG_INFO );
      m_pcWindowHandle->addSubWindow( videoSubWindow );
      videoSubWindow->show();

      connect( subWindow, SIGNAL( aboutToClose( SubWindowAbstract* ) ), m_appModuleVideo,
               SLOT( closeSubWindow( SubWindowAbstract* ) ) );
      connect( subWindow, SIGNAL( zoomFactorChanged_SWindow( const double, const QPoint ) ), m_appModuleVideo,
               SLOT( zoomToFactorAll( double, QPoint ) ) );
      connect( subWindow, SIGNAL( scrollBarMoved_SWindow( const QPoint ) ), m_appModuleVideo,
               SLOT( moveAllScrollBars( const QPoint ) ) );

      videoSubWindow->zoomToFit();
      updateZoomFactorSBox();

      m_appModuleVideo->openSubWindow( videoSubWindow );

      addStreamInfoToRecentList( videoSubWindow->getStreamInfo() );

      printMessage( "File loaded", LOG_INFO );
      m_cLastOpenPath = QFileInfo( fileName ).path();
    }
    else
    {
      videoSubWindow->close();
    }
  }
  catch( PlaYUVerFailure& e )
  {
    videoSubWindow->close();
    QString warningMsg =
        "Cannot open file " + QFileInfo( fileName ).fileName() + " with the following error: \n" + e.what();
    QMessageBox::warning( this, QApplication::applicationName(), warningMsg );
    printMessage( warningMsg, LOG_ERROR );
  }
}

// -----------------------  File Functions  -----------------------

Void PlaYUVerApp::open()
{
  QString supported = tr( "Supported Files (" );
  QStringList formatsList;
  std::vector<PlaYUVerSupportedFormat> supportedFmts = PlaYUVerStream::supportedReadFormats();

  for( UInt i = 0; i < supportedFmts.size(); i++ )
  {
    QString currFmt;
    supported.append( " *." );
    supported.append( QString::fromStdString( supportedFmts[i].formatExt ) );
    currFmt.append( QString::fromStdString( supportedFmts[i].formatName ) );
    currFmt.append( " (*." );
    currFmt.append( QString::fromStdString( supportedFmts[i].formatExt ) );
    currFmt.append( ")" );
    formatsList << currFmt;
  }
  supported.append( " )" );

  QStringList filter;
  filter << supported << formatsList << tr( "All Files (*)" );

  QStringList fileNameList =
      QFileDialog::getOpenFileNames( this, tr( "Open File" ), m_cLastOpenPath, filter.join( ";;" ) );

  for( Int i = 0; i < fileNameList.size(); i++ )
  {
    if( !fileNameList.at( i ).isEmpty() )
    {
      loadFile( fileNameList.at( i ) );
    }
  }
}

Void PlaYUVerApp::openRecent()
{
  QAction* action = qobject_cast<QAction*>( sender() );
  PlaYUVerStreamInfo recentFile = action->data().value<PlaYUVerStreamInfo>();
  if( action )
    loadFile( recentFile.m_cFilename, &recentFile );
}

Void PlaYUVerApp::save()
{
  if( m_pcCurrentVideoSubWindow )
  {
    VideoSubWindow* saveWindow = m_pcCurrentVideoSubWindow;
    QString supported = tr( "Supported Files (" );
    QStringList formatsList;

    std::vector<PlaYUVerSupportedFormat> supportedFmts = PlaYUVerStream::supportedWriteFormats();
    for( UInt i = 0; i < supportedFmts.size(); i++ )
    {
      QString currFmt;
      supported.append( " *." );
      supported.append( QString::fromStdString( supportedFmts[i].formatExt ) );
      currFmt.append( QString::fromStdString( supportedFmts[i].formatName ) );
      currFmt.append( " (*." );
      currFmt.append( QString::fromStdString( supportedFmts[i].formatExt ) );
      currFmt.append( ")" );
      formatsList << currFmt;
    }
    supported.append( " )" );

    QStringList filter;
    filter << supported << formatsList << tr( "All Files (*)" );

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Open File" ), m_cLastOpenPath, filter.join( ";;" ) );

    if( !fileName.isEmpty() )
    {
      m_cLastOpenPath = QFileInfo( fileName ).path();
      try
      {
        saveWindow->save( fileName );
      }
      catch( PlaYUVerFailure& e )
      {
        QApplication::restoreOverrideCursor();
        QString warningMsg =
            "Cannot save file " + QFileInfo( fileName ).fileName() + " with the following error: \n" + e.what();
        QMessageBox::warning( this, QApplication::applicationName(), warningMsg );
        printMessage( warningMsg, LOG_ERROR );
        return;
      }
    }
  }
}

Void PlaYUVerApp::format()
{
  if( m_pcCurrentVideoSubWindow )
  {
    VideoSubWindow* pcVideoSubWindow = m_pcCurrentVideoSubWindow;
    try
    {
      if( pcVideoSubWindow->loadFile( pcVideoSubWindow->getCurrentFileName(), true ) )
      {
        addStreamInfoToRecentList( pcVideoSubWindow->getStreamInfo() );
      }
    }
    catch( PlaYUVerFailure& e )
    {
      QString warningMsg = "Cannot change format of " + QFileInfo( pcVideoSubWindow->getCurrentFileName() ).fileName() +
                           " with the following error: \n" + e.what();
      QMessageBox::warning( this, QApplication::applicationName(), warningMsg );
      printMessage( warningMsg, LOG_ERROR );
      qDebug() << warningMsg;
      m_pcCurrentSubWindow->close();
    }
    m_pcCurrentSubWindow = NULL;
    update();
  }
}

Void PlaYUVerApp::reload()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->refreshSubWindow();
    m_pcCurrentSubWindow = NULL;
    update();
  }
}

Void PlaYUVerApp::reloadAll()
{
  UInt windowCategoryOrder[] = {
      SubWindowAbstract::VIDEO_STREAM_SUBWINDOW, SubWindowAbstract::MODULE_SUBWINDOW,
      SubWindowAbstract::MODULE_SUBWINDOW,
  };

  for( UInt c = 0; c < 3; c++ )
  {
    QList<SubWindowAbstract*> subWindowList = m_pcWindowHandle->findSubWindow( windowCategoryOrder[c] );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      subWindowList.at( i )->refreshSubWindow();
    }
  }
  m_pcCurrentSubWindow = NULL;
  update();
}

Void PlaYUVerApp::loadAll()
{
  if( m_pcCurrentVideoSubWindow )
  {
    printMessage( "Loading file into memory...", LOG_INFO );
    m_pcCurrentVideoSubWindow->loadAll();
    printMessage( "File loaded", LOG_INFO );
  }
}

// -----------------------  Zoom Functions  -----------------------

Void PlaYUVerApp::normalSize()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->normalSize();
    updateZoomFactorSBox();
  }
}

Void PlaYUVerApp::zoomToFit()
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->zoomToFit();
    updateZoomFactorSBox();
  }
}

Void PlaYUVerApp::zoomToFitAll()
{
  VideoSubWindow* videoSubWindow;
  QList<SubWindowAbstract*> subWindowList = m_pcWindowHandle->findSubWindow( SubWindowAbstract::VIDEO_SUBWINDOW );
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    videoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
    videoSubWindow->zoomToFit();
  }
  if( m_pcCurrentSubWindow )
    updateZoomFactorSBox();
}

Void PlaYUVerApp::scaleFrame( int ratio )
{
  if( m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow->scaleView( ( Double )( ratio ) / 100.0 );
    updateZoomFactorSBox();
  }
}

Void PlaYUVerApp::zoomFromSBox( double zoom )
{
  SubWindowAbstract* activeSubWindow = m_pcWindowHandle->activeSubWindow();
  Double factor = zoom / 100;
  if( activeSubWindow )
  {
    activeSubWindow->zoomToFactor( factor );
  }
}

Void PlaYUVerApp::updateZoomFactorSBox()
{
  Double factor;
  if( m_pcCurrentSubWindow )
  {
    factor = m_pcCurrentSubWindow->getScaleFactor();
    m_pcZoomFactorSBox->setValue( factor * 100 );
  }
}

// -----------------------  Drag and drop functions  ----------------------

Void PlaYUVerApp::dragEnterEvent( QDragEnterEvent* event )
{
  // setText(tr("<drop content>"));
  // mdiArea->setBackgroundRole( QPalette::Highlight );
  event->acceptProposedAction();
}

Void PlaYUVerApp::dropEvent( QDropEvent* event )
{
  const QMimeData* mimeData = event->mimeData();
  QList<QUrl> urlList = mimeData->urls();
  if( urlList.size() == 1 )
  {
    QString fileName = urlList.at( 0 ).toLocalFile();
    m_cLastOpenPath = QFileInfo( fileName ).path();
    loadFile( fileName );
  }
}

// -----------------------  Sub Window Functions  -----------------------

VideoSubWindow* PlaYUVerApp::findVideoStreamSubWindow( const PlaYUVerSubWindowHandle* windowManager,
                                                       const QString& fileName )
{
  QString canonicalFilePath = QFileInfo( fileName ).canonicalFilePath();
  VideoSubWindow* pcSubWindow;
  QList<SubWindowAbstract*> subWindowList = windowManager->findSubWindow( SubWindowAbstract::VIDEO_STREAM_SUBWINDOW );
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    pcSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
    if( pcSubWindow->getCurrentFileName() == canonicalFilePath )
      return qobject_cast<VideoSubWindow*>( pcSubWindow );
  }
  return 0;
}

// -----------------------  Update Functions  -----------------------

Void PlaYUVerApp::update()
{
  SubWindowAbstract* activeSubWindow = m_pcWindowHandle->activeSubWindow();
  QCoreApplication::processEvents();
  // if( activeSubWindow() != m_pcCurrentSubWindow )
  {
    m_pcCurrentSubWindow = NULL;
    m_pcCurrentVideoSubWindow = NULL;
    if( activeSubWindow )
    {
      m_pcCurrentSubWindow = activeSubWindow;

      if( m_pcCurrentSubWindow->getCategory() & SubWindowAbstract::VIDEO_SUBWINDOW )
      {
        m_pcCurrentVideoSubWindow = qobject_cast<VideoSubWindow*>( m_pcCurrentSubWindow );
      }

      if( m_pcCurrentVideoSubWindow )
      {
        if( !m_pcWindowHandle->findSubWindow( m_pcCurrentVideoSubWindow->getRefSubWindow() ) )
        {
          m_pcCurrentVideoSubWindow->setRefSubWindow( NULL );
        }
      }
      updateZoomFactorSBox();
    }
  }
  //! Check this - these two function should swap order
  m_appModuleQuality->update( m_pcCurrentVideoSubWindow );
  m_appModuleVideo->update( m_pcCurrentVideoSubWindow );
  updateMenus();
}

Void PlaYUVerApp::printMessage( const QString& msg )
{
  printMessage( msg, 0 );
}

Void PlaYUVerApp::printMessage( const QString& msg, UInt logLevel )
{
  if( !msg.isEmpty() )
  {
    statusBar()->showMessage( msg, 10000 );
    if( logLevel >= LOG_INFO )
    {
      m_pcWindowHandle->processLogMsg( msg );
    }
  }
}

Void PlaYUVerApp::updateMenus()
{
  Bool hasSubWindow = ( m_pcWindowHandle->activeSubWindow() != 0 );

  Bool hasVideoStreamSubWindow = false;
  if( m_pcCurrentSubWindow )
    if( m_pcCurrentSubWindow->getCategory() & SubWindowAbstract::VIDEO_STREAM_SUBWINDOW )
      hasVideoStreamSubWindow = true;

  m_arrayMenu[RECENT_MENU]->setEnabled( m_aRecentFileStreamInfo.size() > 0 ? true : false );

  m_arrayActions[SAVE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[CLOSE_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[CLOSEALL_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[RELOAD_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[RELOAD_ALL_ACT]->setEnabled( hasSubWindow );

  m_arrayActions[FORMAT_ACT]->setEnabled( hasVideoStreamSubWindow );
  m_arrayActions[LOAD_ALL_ACT]->setEnabled( hasVideoStreamSubWindow );

  m_arrayActions[ZOOM_IN_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_OUT_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_NORMAL_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_FIT_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[ZOOM_FIT_ALL_ACT]->setEnabled( hasSubWindow );
  m_pcZoomFactorSBox->setEnabled( hasSubWindow );

  m_appModuleVideo->updateMenus();
  m_appModuleQuality->updateMenus();
  m_appModuleExtensions->updateMenus();
}

// -----------------------  Create Functions  -----------------------

Void PlaYUVerApp::createActions()
{
  m_arrayActions.resize( TOTAL_ACT );

  // ------------ File ------------
  m_arrayActions[OPEN_ACT] = new QAction( QIcon( ":/images/open.png" ), tr( "&Open" ), this );
  m_arrayActions[OPEN_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogOpenButton ) );
  m_arrayActions[OPEN_ACT]->setShortcuts( QKeySequence::Open );
  m_arrayActions[OPEN_ACT]->setStatusTip( tr( "Open stream" ) );
  connect( m_arrayActions[OPEN_ACT], SIGNAL( triggered() ), this, SLOT( open() ) );

  m_arrayRecentFilesActions.resize( MAX_RECENT_FILES );
  for( Int i = 0; i < MAX_RECENT_FILES; i++ )
  {
    m_arrayRecentFilesActions[i] = new QAction( this );
    m_arrayRecentFilesActions[i]->setVisible( false );
    connect( m_arrayRecentFilesActions[i], SIGNAL( triggered() ), this, SLOT( openRecent() ) );
  }

  m_arrayActions[SAVE_ACT] = new QAction( QIcon( ":/images/save.png" ), tr( "&Save Frame" ), this );
  m_arrayActions[SAVE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogSaveButton ) );
  m_arrayActions[SAVE_ACT]->setShortcuts( QKeySequence::SaveAs );
  m_arrayActions[SAVE_ACT]->setStatusTip( tr( "Save current frame" ) );
  connect( m_arrayActions[SAVE_ACT], SIGNAL( triggered() ), this, SLOT( save() ) );

  m_arrayActions[FORMAT_ACT] = new QAction( tr( "&Format" ), this );
  m_arrayActions[FORMAT_ACT]->setIcon( QIcon::fromTheme( "transform-scale", QIcon( ":/images/configuredialog.png" ) ) );
  m_arrayActions[FORMAT_ACT]->setShortcut( Qt::CTRL + Qt::Key_F );
  m_arrayActions[FORMAT_ACT]->setStatusTip( tr( "Open format dialog" ) );
  connect( m_arrayActions[FORMAT_ACT], SIGNAL( triggered() ), this, SLOT( format() ) );

  m_arrayActions[RELOAD_ACT] = new QAction( tr( "&Reload" ), this );
  m_arrayActions[RELOAD_ACT]->setIcon( style()->standardIcon( QStyle::SP_BrowserReload ) );
  m_arrayActions[RELOAD_ACT]->setShortcut( Qt::CTRL + Qt::Key_R );
  m_arrayActions[RELOAD_ACT]->setShortcut( Qt::Key_F5 );
  m_arrayActions[RELOAD_ACT]->setStatusTip( tr( "Reload current sequence" ) );
  connect( m_arrayActions[RELOAD_ACT], SIGNAL( triggered() ), this, SLOT( reload() ) );

  m_arrayActions[RELOAD_ALL_ACT] = new QAction( tr( "Reload All" ), this );
  m_arrayActions[RELOAD_ALL_ACT]->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_R );
  m_arrayActions[RELOAD_ALL_ACT]->setStatusTip( tr( "Reload all sequences" ) );
  connect( m_arrayActions[RELOAD_ALL_ACT], SIGNAL( triggered() ), this, SLOT( reloadAll() ) );

  m_arrayActions[LOAD_ALL_ACT] = new QAction( tr( "Preload" ), this );
  m_arrayActions[LOAD_ALL_ACT]->setStatusTip( tr( "Load sequence into memory (caution)" ) );
  connect( m_arrayActions[LOAD_ALL_ACT], SIGNAL( triggered() ), this, SLOT( loadAll() ) );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "&Close" ), this );
  m_arrayActions[CLOSE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogCloseButton ) );
  m_arrayActions[CLOSE_ACT]->setStatusTip( tr( "Close the active window" ) );
  connect( m_arrayActions[CLOSE_ACT], SIGNAL( triggered() ), m_pcWindowHandle, SLOT( removeActiveSubWindow() ) );

  m_arrayActions[CLOSEALL_ACT] = new QAction( tr( "Close &All" ), this );
  m_arrayActions[CLOSEALL_ACT]->setStatusTip( tr( "Close all the windows" ) );
  connect( m_arrayActions[CLOSEALL_ACT], SIGNAL( triggered() ), m_pcWindowHandle, SLOT( removeAllSubWindow() ) );

  m_arrayActions[EXIT_ACT] = new QAction( tr( "E&xit" ), this );
  m_arrayActions[EXIT_ACT]->setShortcuts( QKeySequence::Quit );
  m_arrayActions[EXIT_ACT]->setStatusTip( tr( "Exit the application" ) );
  connect( m_arrayActions[EXIT_ACT], SIGNAL( triggered() ), qApp, SLOT( closeAllWindows() ) );

  // ------------ View ------------
  mapperZoom = new QSignalMapper( this );
  connect( mapperZoom, SIGNAL( mapped( int ) ), this, SLOT( scaleFrame( int ) ) );

  m_arrayActions[ZOOM_IN_ACT] = new QAction( tr( "Zoom &In (+25%)" ), this );
  m_arrayActions[ZOOM_IN_ACT]->setIcon( QIcon::fromTheme( "zoom-in", QIcon( ":/images/zoomin.png" ) ) );
  m_arrayActions[ZOOM_IN_ACT]->setShortcut( tr( "Ctrl++" ) );
  m_arrayActions[ZOOM_IN_ACT]->setStatusTip( tr( "Scale the image up by 25%" ) );
  connect( m_arrayActions[ZOOM_IN_ACT], SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( m_arrayActions[ZOOM_IN_ACT], 125 );

  m_arrayActions[ZOOM_OUT_ACT] = new QAction( tr( "Zoom &Out (-25%)" ), this );
  m_arrayActions[ZOOM_OUT_ACT]->setIcon( QIcon::fromTheme( "zoom-out", QIcon( ":/images/zoomout.png" ) ) );
  m_arrayActions[ZOOM_OUT_ACT]->setShortcut( tr( "Ctrl+-" ) );
  m_arrayActions[ZOOM_OUT_ACT]->setStatusTip( tr( "Scale the image down by 25%" ) );
  connect( m_arrayActions[ZOOM_OUT_ACT], SIGNAL( triggered() ), mapperZoom, SLOT( map() ) );
  mapperZoom->setMapping( m_arrayActions[ZOOM_OUT_ACT], 80 );

  m_arrayActions[ZOOM_NORMAL_ACT] = new QAction( tr( "&Normal Size" ), this );
  m_arrayActions[ZOOM_NORMAL_ACT]->setIcon( QIcon::fromTheme( "zoom-original", QIcon( ":/images/zoomtonormal.png" ) ) );
  m_arrayActions[ZOOM_NORMAL_ACT]->setShortcut( tr( "Ctrl+N" ) );
  m_arrayActions[ZOOM_NORMAL_ACT]->setStatusTip( tr( "Show the image at its original size" ) );
  connect( m_arrayActions[ZOOM_NORMAL_ACT], SIGNAL( triggered() ), this, SLOT( normalSize() ) );

  m_arrayActions[ZOOM_FIT_ACT] = new QAction( tr( "Zoom to &Fit" ), this );
  m_arrayActions[ZOOM_FIT_ACT]->setIcon( QIcon::fromTheme( "zoom-fit-best", QIcon( ":/images/fittowindow.png" ) ) );
  m_arrayActions[ZOOM_FIT_ACT]->setStatusTip( tr( "Zoom in or out to fit on the window." ) );
  connect( m_arrayActions[ZOOM_FIT_ACT], SIGNAL( triggered() ), this, SLOT( zoomToFit() ) );

  m_arrayActions[ZOOM_FIT_ALL_ACT] = new QAction( tr( "Zoom to Fit All" ), this );
  // m_arrayActions[ZOOM_FIT_ALL_ACT]->setIcon( QIcon::fromTheme( "zoom-fit-best", QIcon( ":/images/fittowindow.png" ) )
  // );
  m_arrayActions[ZOOM_FIT_ALL_ACT]->setStatusTip( tr( "Apply zoom to fit to all windows" ) );
  connect( m_arrayActions[ZOOM_FIT_ALL_ACT], SIGNAL( triggered() ), this, SLOT( zoomToFitAll() ) );

  m_appModuleVideo->createActions();
  m_appModuleQuality->createActions();
  m_appModuleExtensions->createActions();
  m_pcWindowHandle->createActions();

// ------------ About ------------

#ifdef USE_FERVOR
  m_arrayActions[UPDATE_ACT] = new QAction( tr( "&Update" ), this );
  m_arrayActions[UPDATE_ACT]->setStatusTip( tr( "Check for updates" ) );
  connect( m_arrayActions[UPDATE_ACT], SIGNAL( triggered() ), FvUpdater::sharedUpdater(),
           SLOT( CheckForUpdatesNotSilent() ) );
#endif

  m_arrayActions[ABOUT_ACT] = new QAction( tr( "&About" ), this );
  m_arrayActions[ABOUT_ACT]->setStatusTip( tr( "Show the application's About box" ) );
  connect( m_arrayActions[ABOUT_ACT], SIGNAL( triggered() ), this, SLOT( about() ) );

  m_arrayActions[ABOUTQT_ACT] = new QAction( tr( "About &Qt" ), this );
  m_arrayActions[ABOUTQT_ACT]->setIcon( QIcon( ":images/qt.png" ) );
  m_arrayActions[ABOUTQT_ACT]->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( m_arrayActions[ABOUTQT_ACT], SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
}

Void PlaYUVerApp::createMenus()
{
  m_arrayMenu.resize( TOTAL_MENUS );

  m_arrayMenu[FILE_MENU] = menuBar()->addMenu( tr( "&File" ) );
  m_arrayMenu[FILE_MENU]->addAction( m_arrayActions[OPEN_ACT] );
  m_arrayMenu[RECENT_MENU] = m_arrayMenu[FILE_MENU]->addMenu( tr( "Open Recent" ) );
  for( Int i = 0; i < MAX_RECENT_FILES; ++i )
  {
    m_arrayMenu[RECENT_MENU]->addAction( m_arrayRecentFilesActions[i] );
  }

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
  m_arrayMenu[VIEW_MENU]->addAction( m_arrayActions[ZOOM_FIT_ALL_ACT] );

  m_arrayMenu[VIEW_MENU]->addSeparator();

  // createPopupMenu() Returns a popup menu containing checkable entries for
  // the toolbars and dock widgets present in the main window.
  m_arrayMenu[DOCK_VIEW_MENU] = createPopupMenu();
  if( m_arrayMenu[DOCK_VIEW_MENU] )
  {
    QAction* actionPopupMenu = m_arrayMenu[VIEW_MENU]->addMenu( m_arrayMenu[DOCK_VIEW_MENU] );
    actionPopupMenu->setText( tr( "&Toolbars/Docks" ) );
  }

  menuBar()->addMenu( m_appModuleVideo->createVideoMenu() );
  menuBar()->addMenu( m_appModuleVideo->createImageMenu() );
  QMenu* QualityMenu = m_appModuleQuality->createMenu();
  menuBar()->addMenu( QualityMenu );
  QMenu* ModuleMenu = m_appModuleExtensions->createMenu();
  menuBar()->addMenu( ModuleMenu );
  QMenu* windowMenu = m_pcWindowHandle->createMenu();
  menuBar()->addMenu( windowMenu );

  menuBar()->addSeparator();

  m_arrayMenu[ABOUT_MENU] = menuBar()->addMenu( tr( "&Help" ) );
#ifdef USE_FERVOR
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[UPDATE_ACT] );
#endif
  m_arrayMenu[ABOUT_MENU]->addSeparator();
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUT_ACT] );
  m_arrayMenu[ABOUT_MENU]->addAction( m_arrayActions[ABOUTQT_ACT] );
}

Void PlaYUVerApp::createToolBars()
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
  m_pcZoomFactorSBox->setDecimals( 0 );
  m_pcZoomFactorSBox->setSingleStep( 10.0 );
  m_pcZoomFactorSBox->setValue( 100.0 );
  m_pcZoomFactorSBox->setSuffix( "%" );
  connect( m_pcZoomFactorSBox, SIGNAL( valueChanged( double ) ), this, SLOT( zoomFromSBox( double ) ) );
  m_arrayToolBars[VIEW_TOOLBAR]->addWidget( m_pcZoomFactorSBox );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_IN_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_OUT_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_NORMAL_ACT] );
  m_arrayToolBars[VIEW_TOOLBAR]->addAction( m_arrayActions[ZOOM_FIT_ACT] );

  m_arrayToolBars[FILE_TOOLBAR]->setMovable( false );
  addToolBar( Qt::TopToolBarArea, m_arrayToolBars[VIEW_TOOLBAR] );

  addToolBar( Qt::TopToolBarArea, m_appModuleVideo->createToolBar() );
}

Void PlaYUVerApp::createDockWidgets()
{
  // Properties Dock Window
  m_arraySideBars.resize( TOTAL_DOCK );
  addDockWidget( Qt::RightDockWidgetArea, m_appModuleVideo->createDock() );
  addDockWidget( Qt::RightDockWidgetArea, m_appModuleQuality->createDock() );
}

Void PlaYUVerApp::createStatusBar()
{
  //! Warning: the following widget cannot change size too much
  statusBar()->addPermanentWidget( m_appModuleVideo->createStatusBarMessage() );
  printMessage( "Ready!", LOG_INFO );
}

Void PlaYUVerApp::addStreamInfoToRecentList( PlaYUVerStreamInfo streamInfo )
{
  Int idx = findPlaYUVerStreamInfo( m_aRecentFileStreamInfo, streamInfo.m_cFilename );
  if( idx >= 0 )
    m_aRecentFileStreamInfo.remove( idx );
  m_aRecentFileStreamInfo.prepend( streamInfo );
  while( m_aRecentFileStreamInfo.size() > MAX_RECENT_FILES )
    m_aRecentFileStreamInfo.remove( m_aRecentFileStreamInfo.size() - 1 );
  updateRecentFileActions();
}

Void PlaYUVerApp::updateRecentFileActions()
{
  Int numRecentFiles = m_aRecentFileStreamInfo.size();
  numRecentFiles = qMin( numRecentFiles, MAX_RECENT_FILES );
  Int actionIdx = 0;
  while( actionIdx < numRecentFiles )
  {
    QString text = m_aRecentFileStreamInfo.at( actionIdx ).m_cFilename;
    m_arrayRecentFilesActions.at( actionIdx )->setText( QFileInfo( text ).fileName() );
    m_arrayRecentFilesActions.at( actionIdx )->setToolTip( "Open File " + text );
    m_arrayRecentFilesActions.at( actionIdx )->setStatusTip( "Open File " + text );
    m_arrayRecentFilesActions.at( actionIdx )
        ->setData( QVariant::fromValue( m_aRecentFileStreamInfo.at( actionIdx ) ) );
    m_arrayRecentFilesActions.at( actionIdx )->setVisible( true );
    actionIdx++;
  }
  while( actionIdx < MAX_RECENT_FILES )
  {
    m_arrayRecentFilesActions.at( actionIdx )->setVisible( false );
    actionIdx++;
  }
  m_arrayMenu[RECENT_MENU]->setEnabled( m_aRecentFileStreamInfo.size() > 0 ? true : false );
}

Void PlaYUVerApp::checkRecentFileActions()
{
  Int i = 0;
  while( i < m_aRecentFileStreamInfo.size() )
  {
    if( !QFileInfo( m_aRecentFileStreamInfo.at( i ).m_cFilename ).exists() )
    {
      m_aRecentFileStreamInfo.remove( i );
      continue;
    }
    else
    {
      UInt64 fileSize = QFileInfo( m_aRecentFileStreamInfo.at( i ).m_cFilename ).size();
      if( m_aRecentFileStreamInfo.at( i ).m_uiFileSize != fileSize )
      {
        m_aRecentFileStreamInfo.remove( i );
        continue;
      }
    }
    i++;
  }
}

Void PlaYUVerApp::readSettings()
{
  QSettings appSettings;

  QPoint pos = appSettings.value( "MainWindow/Position", QPoint( 200, 200 ) ).toPoint();
  QSize size = appSettings.value( "MainWindow/Size", QSize( 500, 400 ) ).toSize();
  move( pos );
  resize( size );

  m_cLastOpenPath = appSettings.value( "MainWindow/LastOpenPath", QDir::homePath() ).toString();

  QVariant value = appSettings.value( "MainWindow/RecentFileList" );
  m_aRecentFileStreamInfo = value.value<PlaYUVerStreamInfoVector>();
  checkRecentFileActions();
  updateRecentFileActions();

  m_appModuleVideo->readSettings();
  m_appModuleQuality->readSettings();
  m_appModuleExtensions->readSettings();
  m_pcWindowHandle->readSettings();
}

Void PlaYUVerApp::writeSettings()
{
  QSettings appSettings;

  appSettings.setValue( "MainWindow/Position", pos() );
  appSettings.setValue( "MainWindow/Size", size() );
  appSettings.setValue( "MainWindow/LastOpenPath", m_cLastOpenPath );

  QVariant var;
  var.setValue<PlaYUVerStreamInfoVector>( m_aRecentFileStreamInfo );
  appSettings.setValue( "MainWindow/RecentFileList", var );

  m_appModuleVideo->writeSettings();
  m_appModuleQuality->writeSettings();
  m_appModuleExtensions->writeSettings();
  m_pcWindowHandle->writeSettings();
}
