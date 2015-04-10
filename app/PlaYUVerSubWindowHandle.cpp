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
 * \file     PlaYUVerSubWindowHandle.cpp
 * \brief    Handle multiple sub-windows
 */

#include "PlaYUVerApp.h"
#include "PlaYUVerSubWindowHandle.h"
#include "PlaYUVerMdiSubWindow.h"
#include "SubWindowAbstract.h"

namespace plaYUVer
{

class PlaYUVerMdiArea: public QMdiArea
{
public:
  PlaYUVerMdiArea( QWidget *parent ) :
          QMdiArea( parent ),
          m_pixmapLogo( ":/images/playuver-backgroud-logo.png" )
  {
    setBackgroundRole( QPalette::Background );
  }
protected:
  void paintEvent( QPaintEvent *event )
  {
    QMdiArea::paintEvent( event );
    QPainter painter( viewport() );

    QPixmap pixFinalLogo = m_pixmapLogo.scaled( 2 * size() / 3, Qt::KeepAspectRatio );

    // Calculate the logo position - the bottom right corner of the mdi area.
    int x = width() / 2 - pixFinalLogo.width() / 2;
    int y = height() / 2 - pixFinalLogo.height() / 2;
    painter.drawPixmap( x, y, pixFinalLogo );
  }
private:
  QPixmap m_pixmapLogo;
};

PlaYUVerSubWindowHandle::PlaYUVerSubWindowHandle( QWidget *parent ) :
        QWidget( parent )
{
  m_iWindowMode = -1;
  m_pcWindowManagerLayout = NULL;
  m_pcMdiArea = NULL;
  m_pcActiveWindow = NULL;
  m_apcSubWindowList.clear();
  m_apcMdiSubWindowList.clear();

  m_pcApp = qobject_cast<PlaYUVerApp*>( parentWidget() );
  //setWindowMode( NormalSubWindows );
  setWindowMode( MdiWSubWindows );
}

Void PlaYUVerSubWindowHandle::resetWindowMode()
{
  QList<SubWindowAbstract*> apcSubWindowList = m_apcSubWindowList;

  if( m_iWindowMode == NormalSubWindows )
  {

  }
  if( m_iWindowMode == MdiWSubWindows )
  {
    m_cMdiModeWindowPosition = parentWidget()->pos();
    m_cMdiModeWindowSize = parentWidget()->size();
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      m_apcSubWindowList.at( i )->setParent( NULL );
    }
    for( Int i = 0; i < m_apcMdiSubWindowList.size(); i++ )
    {
      disconnect( m_apcMdiSubWindowList.at( i ), SIGNAL( aboutToClose( PlaYUVerMdiSubWindow* ) ), this, SLOT( removeMdiSubWindow( PlaYUVerMdiSubWindow* ) ) );
      m_apcMdiSubWindowList.at( i )->close();
    }
    m_apcMdiSubWindowList.clear();
    if( m_pcMdiArea )
      delete m_pcMdiArea;
    if( m_pcWindowManagerLayout )
      delete m_pcWindowManagerLayout;

  }
}

Void PlaYUVerSubWindowHandle::setWindowMode( Int iWindowMode )
{
  if( m_iWindowMode == iWindowMode )
    return;

  resetWindowMode();

  if( iWindowMode == NormalSubWindows )
  {
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      m_apcSubWindowList.at( i )->setParent( NULL );
      m_apcSubWindowList.at( i )->resize( m_apcSubWindowList.at( i )->sizeHint() );
      m_apcSubWindowList.at( i )->show();
    }
    setVisible( false );
    parentWidget()->update();
    parentWidget()->showNormal();
    qApp->processEvents();
    QSize screenSize = QApplication::desktop()->availableGeometry().size();
    parentWidget()->move( 0, 0 );
    parentWidget()->resize( screenSize.width(), 5 );
    parentWidget()->setWindowTitle( QApplication::applicationName() );
  }

  if( iWindowMode == MdiWSubWindows )
  {
    setVisible( true );
    m_pcMdiArea = new PlaYUVerMdiArea( this );
    m_pcMdiArea->setActivationOrder( QMdiArea::ActivationHistoryOrder );
    connect( m_pcMdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( updateActiveSubWindow() ) );
    m_pcWindowManagerLayout = new QHBoxLayout( this );
    m_pcWindowManagerLayout->addWidget( m_pcMdiArea );
    setLayout( m_pcWindowManagerLayout );
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      addMdiSubWindow( m_apcSubWindowList.at( i ) );
      m_apcSubWindowList.at( i )->resize( m_apcSubWindowList.at( i )->sizeHint() );
    }
    parentWidget()->move( m_cMdiModeWindowPosition );
    parentWidget()->resize( m_cMdiModeWindowSize );
    if( m_pcActiveWindow )
    {
      parentWidget()->setWindowTitle( QApplication::applicationName() + " :: " + m_pcActiveWindow->getWindowName() );
    }
  }
  m_iWindowMode = iWindowMode;
  //tileSubWindows();
  parentWidget()->update();
  qApp->processEvents();
}

Void PlaYUVerSubWindowHandle::updateActiveSubWindow( SubWindowAbstract *window )
{
  if( m_iWindowMode == NormalSubWindows )
  {
    if( window )
    {
      m_pcActiveWindow = window;
      emit changed();
    }
  }

  if( m_iWindowMode == MdiWSubWindows )
  {
    m_pcActiveWindow = NULL;
    QMdiSubWindow* mdiSubWindow = m_pcMdiArea->activeSubWindow();
    if( mdiSubWindow )
    {
      QWidget* activeWidget = m_pcMdiArea->activeSubWindow()->widget();
      Int windowIdx = m_apcSubWindowList.indexOf( qobject_cast<SubWindowAbstract*>( activeWidget ) );
      if( windowIdx >= 0 )
      {
        m_pcActiveWindow = m_apcSubWindowList.at( windowIdx );
        emit changed();
      }
    }
    if( m_pcActiveWindow )
    {
      parentWidget()->setWindowTitle( QApplication::applicationName() + " :: " + m_pcActiveWindow->getWindowName() );
    }
  }
}

Void PlaYUVerSubWindowHandle::addMdiSubWindow( SubWindowAbstract *window )
{
  PlaYUVerMdiSubWindow* mdiSubWindow = new PlaYUVerMdiSubWindow;
  mdiSubWindow->setWidget( window );
  m_pcMdiArea->addSubWindow( mdiSubWindow );
  m_apcMdiSubWindowList.append( mdiSubWindow );
  window->setSubWindow( mdiSubWindow );
  connect( mdiSubWindow, SIGNAL( aboutToClose( PlaYUVerMdiSubWindow* ) ), this, SLOT( removeMdiSubWindow( PlaYUVerMdiSubWindow* ) ) );
}

Void PlaYUVerSubWindowHandle::addSubWindow( SubWindowAbstract *window, Qt::WindowFlags flags )
{
  if( window )
  {
    connect( window, SIGNAL( updateStatusBar( const QString& ) ), m_pcApp, SLOT( updateStatusBar( const QString& ) ) );
    connect( window, SIGNAL( zoomFactorChanged_SWindow( const double, const QPoint ) ), m_pcApp, SLOT( updateZoomFactorSBox() ) );
    if( m_iWindowMode == NormalSubWindows )
    {
      connect( window, SIGNAL( aboutToActivate( SubWindowAbstract* ) ), this, SLOT( updateActiveSubWindow( SubWindowAbstract* ) ) );
      connect( window, SIGNAL( aboutToClose( SubWindowAbstract* ) ), this, SLOT( removeSubWindow( SubWindowAbstract* ) ) );
    }
    if( m_iWindowMode == MdiWSubWindows )
    {
      addMdiSubWindow( window );
    }
    m_apcSubWindowList.append( window );
  }
}

Void PlaYUVerSubWindowHandle::removeSubWindow( Int windowIdx )
{
  if( windowIdx >= 0 )
  {
    if( m_iWindowMode == NormalSubWindows )
    {
      SubWindowAbstract* subWindow = m_apcSubWindowList.at( windowIdx );
      m_apcSubWindowList.removeAt( windowIdx );
      subWindow->close();
    }
    if( m_iWindowMode == MdiWSubWindows )
    {
      PlaYUVerMdiSubWindow* mdiSubWindow = m_apcMdiSubWindowList.at( windowIdx );
      m_apcMdiSubWindowList.removeAt( windowIdx );
      m_pcMdiArea->removeSubWindow( m_apcSubWindowList.at( windowIdx ) );
      mdiSubWindow->close();
      m_apcSubWindowList.removeAt( windowIdx );
    }

    if( !m_apcSubWindowList.isEmpty() )
      m_apcSubWindowList.front()->setFocus();
    else
      m_pcActiveWindow = NULL;

    emit changed();
  }
}

Void PlaYUVerSubWindowHandle::removeSubWindow( SubWindowAbstract* window )
{
  removeSubWindow( m_apcSubWindowList.indexOf( window ) );
}

Void PlaYUVerSubWindowHandle::removeMdiSubWindow( PlaYUVerMdiSubWindow* window )
{
  removeSubWindow( m_apcMdiSubWindowList.indexOf( window ) );
}

Void PlaYUVerSubWindowHandle::removeAllSubWindow()
{
  while( m_apcSubWindowList.size() > 0 )
  {
    removeSubWindow( 0 );
  }
}

Void PlaYUVerSubWindowHandle::removeActiveSubWindow()
{
  removeSubWindow( m_pcActiveWindow );
}

SubWindowAbstract* PlaYUVerSubWindowHandle::activeSubWindow() const
{
  return m_pcActiveWindow;
}

QList<SubWindowAbstract*> PlaYUVerSubWindowHandle::findSubWindow( const UInt uiCategory ) const
{
  QList<SubWindowAbstract*> apcSubWindowList;
  if( uiCategory == 0 )
  {
    apcSubWindowList = m_apcSubWindowList;
  }
  else
  {
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      if( m_apcSubWindowList.at( i )->getCategory() & uiCategory )
        apcSubWindowList.append( m_apcSubWindowList.at( i ) );
    }
  }
  return apcSubWindowList;
}

QList<SubWindowAbstract*> PlaYUVerSubWindowHandle::findSubWindow( const QString &windowName, const UInt uiCategory ) const
{
  QList<SubWindowAbstract*> subWindowList = findSubWindow( uiCategory );
  QList<SubWindowAbstract*> apcSubWindowList;
  if( !windowName.isEmpty() )
  {
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      if( subWindowList.at( i )->getWindowName() == windowName )
        apcSubWindowList.append( subWindowList.at( i ) );
    }
  }
  return apcSubWindowList;
}

SubWindowAbstract* PlaYUVerSubWindowHandle::findSubWindow( const SubWindowAbstract* subWindow ) const
{
  QList<SubWindowAbstract*> subWindowList = findSubWindow();
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    if( subWindow == subWindowList.at( i ) )
      return subWindowList.at( i );
  }
  return 0;
}

Void PlaYUVerSubWindowHandle::setActiveSubWindow( QWidget *window )
{
  if( !window )
    return;

  m_pcActiveWindow = qobject_cast<SubWindowAbstract *>( window );

  if( m_iWindowMode == NormalSubWindows )
  {
    m_pcActiveWindow->window()->activateWindow();
  }
  if( m_iWindowMode == MdiWSubWindows )
  {
    m_pcActiveWindow->setFocus();
  }
}

Void PlaYUVerSubWindowHandle::tileSubWindows()
{
  UInt numberOfWindows = m_apcSubWindowList.size();
  if( numberOfWindows == 0 )
    return;

  if( m_iWindowMode == NormalSubWindows )
  {
    QSize screenSize = QApplication::desktop()->availableGeometry().size();
    UInt minWindowsInRow = 1;
    while( ( screenSize.width() / minWindowsInRow ) > 500 )
    {
      minWindowsInRow++;
    }
    parentWidget()->showNormal();
    qApp->processEvents();
    parentWidget()->move( 0, 0 );
    parentWidget()->resize( screenSize.width(), 5 );
    QSize appSize = m_pcApp->size();
    QPoint topLeftCornerWindow( 0, appSize.height() );
    //Select the arrangement based on the number of windows

    if( numberOfWindows % 2 != 0 && numberOfWindows > minWindowsInRow )
    {
      numberOfWindows++;
    }
    UInt numberTillingRows = 1;
    UInt numberTillingColumns;
    if( numberOfWindows > minWindowsInRow )
    {
      numberTillingColumns = minWindowsInRow;
      for( Int i = numberOfWindows; i > 0; )
      {
        numberTillingRows = ( numberOfWindows + numberTillingColumns - 1 ) / numberTillingColumns;
        if( numberTillingColumns > numberTillingRows )
        {
          break;
        }
        numberTillingColumns++;
      }
    }
    else
    {
      numberTillingColumns = numberOfWindows;
    }
    if( numberOfWindows / numberTillingRows == 1 )
    {
      numberTillingColumns = numberTillingRows;
    }

    QSize totalWindowsSize( screenSize.width(), screenSize.height() - topLeftCornerWindow.y() );
    QSize windowsSize( totalWindowsSize.width() / numberTillingColumns, totalWindowsSize.height() / numberTillingRows );
    QPoint windowRelativePos;
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      windowRelativePos.setY( ( i / numberTillingColumns ) * windowsSize.height() );
      windowRelativePos.setX( ( i % numberTillingColumns ) * windowsSize.width() );
      m_apcSubWindowList.at( i )->resize( windowsSize );
      m_apcSubWindowList.at( i )->move( topLeftCornerWindow + windowRelativePos );
    }
  }
  if( m_iWindowMode == MdiWSubWindows )
  {
    m_pcMdiArea->tileSubWindows();
  }

}

Void PlaYUVerSubWindowHandle::cascadeSubWindows()
{
  if( m_iWindowMode == MdiWSubWindows )
  {
    m_pcMdiArea->cascadeSubWindows();
  }
}

Void PlaYUVerSubWindowHandle::createActions()
{
  m_mapperWindow = new QSignalMapper( this );
  connect( m_mapperWindow, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

  m_arrayActions.resize( TOTAL_ACT );

  // ------------ Tools ------------
  m_actionGroupWindowMode = new QActionGroup( this );
  m_actionGroupWindowMode->setExclusive( true );

  m_mapperWindowMode = new QSignalMapper( this );
  connect( m_mapperWindowMode, SIGNAL( mapped(int) ), this, SLOT( setWindowMode(int) ) );

  m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT] = new QAction( tr( "&Detached SubWindows" ), this );
  m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT]->setCheckable( true );
  m_actionGroupWindowMode->addAction( m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT] );
  connect( m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT], SIGNAL( triggered() ), m_mapperWindowMode, SLOT( map() ) );
  m_mapperWindowMode->setMapping( m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT], NormalSubWindows );

  m_arrayActions[MDI_SUBWINDOW_MODE_ACT] = new QAction( "&MDI SubWindows", this );
  m_arrayActions[MDI_SUBWINDOW_MODE_ACT]->setCheckable( true );
  m_actionGroupWindowMode->addAction( m_arrayActions[MDI_SUBWINDOW_MODE_ACT] );
  connect( m_arrayActions[MDI_SUBWINDOW_MODE_ACT], SIGNAL( triggered() ), m_mapperWindowMode, SLOT( map() ) );
  m_mapperWindowMode->setMapping( m_arrayActions[MDI_SUBWINDOW_MODE_ACT], MdiWSubWindows );

  m_actionGroupWindowMode->actions().at( m_iWindowMode )->setChecked( true );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "&Close" ), this );
  m_arrayActions[CLOSE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogCloseButton ) );
  m_arrayActions[CLOSE_ACT]->setStatusTip( tr( "Close the active window" ) );
  connect( m_arrayActions[CLOSE_ACT], SIGNAL( triggered() ), this, SLOT( removeActiveSubWindow() ) );

  m_arrayActions[CLOSE_ACT] = new QAction( tr( "&Close" ), this );
  m_arrayActions[CLOSE_ACT]->setIcon( style()->standardIcon( QStyle::SP_DialogCloseButton ) );
  m_arrayActions[CLOSE_ACT]->setStatusTip( tr( "Close the active window" ) );
  connect( m_arrayActions[CLOSE_ACT], SIGNAL( triggered() ), this, SLOT( removeActiveSubWindow() ) );

  m_arrayActions[CLOSE_ALL_ACT] = new QAction( tr( "Close &All" ), this );
  m_arrayActions[CLOSE_ALL_ACT]->setStatusTip( tr( "Close all the windows" ) );
  connect( m_arrayActions[CLOSE_ALL_ACT], SIGNAL( triggered() ), this, SLOT( removeAllSubWindow() ) );

  m_arrayActions[TILE_WINDOWS_ACT] = new QAction( tr( "Tile" ), this );
  m_arrayActions[TILE_WINDOWS_ACT]->setIcon( QIcon( ":images/windowstile.png" ) );
  m_arrayActions[TILE_WINDOWS_ACT]->setStatusTip( tr( "Tile the windows" ) );
//connect( m_arrayActions[TILE_WINDOWS_ACT], SIGNAL( triggered() ), mdiArea, SLOT( tileSubWindows() ) );
  connect( m_arrayActions[TILE_WINDOWS_ACT], SIGNAL( triggered() ), this, SLOT( tileSubWindows() ) );

  m_arrayActions[CASCADE_WINDOWS_ACT] = new QAction( tr( "Cascade" ), this );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setIcon( QIcon( ":images/windowscascade.png" ) );
  m_arrayActions[CASCADE_WINDOWS_ACT]->setStatusTip( tr( "Cascade the windows" ) );
  connect( m_arrayActions[CASCADE_WINDOWS_ACT], SIGNAL( triggered() ), this, SLOT( cascadeSubWindows() ) );

  m_arrayActions[NEXT_WINDOWS_ACT] = new QAction( tr( "Ne&xt" ), this );
  m_arrayActions[NEXT_WINDOWS_ACT]->setShortcuts( QKeySequence::NextChild );
  m_arrayActions[NEXT_WINDOWS_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_ArrowRight ) ) );
  m_arrayActions[NEXT_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the next window" ) );
//  connect( m_arrayActions[NEXT_WINDOWS_ACT], SIGNAL( triggered() ), this, SLOT( activateNextSubWindow() ) );

  m_arrayActions[PREVIOUS_WINDOWS_ACT] = new QAction( tr( "Pre&vious" ), this );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setShortcuts( QKeySequence::PreviousChild );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setIcon( QIcon( style()->standardIcon( QStyle::SP_ArrowLeft ) ) );
  m_arrayActions[PREVIOUS_WINDOWS_ACT]->setStatusTip( tr( "Move the focus to the previous window" ) );
//  connect( m_arrayActions[PREVIOUS_WINDOWS_ACT], SIGNAL( triggered() ), this, SLOT( activatePreviousSubWindow() ) );

  m_arrayActions[SEPARATOR_ACT] = new QAction( this );
  m_arrayActions[SEPARATOR_ACT]->setSeparator( true );

}

QMenu* PlaYUVerSubWindowHandle::createMenu()
{
  m_pcMenuWindow = new QMenu( "&Window", this );
  updateMenu();
  connect( m_pcMenuWindow, SIGNAL( aboutToShow() ), this, SLOT( updateMenu() ) );
  return m_pcMenuWindow;
}

Void PlaYUVerSubWindowHandle::updateMenu()
{
  m_pcMenuWindow->clear();
  m_pcMenuWindow->addAction( m_arrayActions[NORMAL_SUBWINDOW_MODE_ACT] );
  m_pcMenuWindow->addAction( m_arrayActions[MDI_SUBWINDOW_MODE_ACT] );
  m_pcMenuWindow->addSeparator();
  m_pcMenuWindow->addAction( m_arrayActions[CLOSE_ACT] );
  m_pcMenuWindow->addAction( m_arrayActions[CLOSE_ALL_ACT] );
  m_pcMenuWindow->addSeparator();
  m_pcMenuWindow->addAction( m_arrayActions[TILE_WINDOWS_ACT] );
  m_pcMenuWindow->addAction( m_arrayActions[CASCADE_WINDOWS_ACT] );
  m_pcMenuWindow->addSeparator();
  m_pcMenuWindow->addAction( m_arrayActions[NEXT_WINDOWS_ACT] );
  m_pcMenuWindow->addAction( m_arrayActions[PREVIOUS_WINDOWS_ACT] );
  m_pcMenuWindow->addSeparator();

  Int number_windows = m_apcSubWindowList.size();
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

  m_arrayActions[SEPARATOR_ACT]->setVisible( number_windows > 0 );

  for( Int i = 0; i < number_windows; ++i )
  {
    SubWindowAbstract *subWindow = m_apcSubWindowList.at( i );

    QString text;
    if( i < 9 )
    {
      text = tr( "&%1 %2" ).arg( i + 1 ).arg( subWindow->getWindowName() );
    }
    else
    {
      text = tr( "%1 %2" ).arg( i + 1 ).arg( subWindow->getWindowName() );
    }
    QAction *action = m_pcMenuWindow->addAction( text );
    action->setCheckable( true );
    action->setChecked( subWindow == activeSubWindow() );
    connect( action, SIGNAL( triggered() ), m_mapperWindow, SLOT( map() ) );
    m_mapperWindow->setMapping( action, subWindow );
  }
}

Void PlaYUVerSubWindowHandle::readSettings()
{
  QSettings appSettings;
  m_cMdiModeWindowPosition = appSettings.value( "SubWindowManager/LastMdiPosition", QPoint( 200, 200 ) ).toPoint();
  m_cMdiModeWindowSize = appSettings.value( "SubWindowManager/LastMdiSize", QSize( 500, 400 ) ).toSize();
  Int iWindowMode = appSettings.value( "SubWindowManager/SubWindowMode", MdiWSubWindows ).toInt();
  setWindowMode( iWindowMode );
  m_actionGroupWindowMode->actions().at( iWindowMode )->setChecked( true );
}

Void PlaYUVerSubWindowHandle::writeSettings()
{
  QSettings appSettings;
  appSettings.setValue( "SubWindowManager/LastMdiPosition", m_cMdiModeWindowPosition );
  appSettings.setValue( "SubWindowManager/LastMdiSize", m_cMdiModeWindowSize );
  appSettings.setValue( "SubWindowManager/SubWindowMode", m_iWindowMode );
}

}  // NAMESPACE
