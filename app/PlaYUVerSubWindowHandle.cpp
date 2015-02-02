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

#include "plaYUVerApp.h"
#include "PlaYUVerSubWindowHandle.h"
#include "PlaYUVerMdiSubWindow.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

class PlaYUVerMdiArea: public QMdiArea
{
public:
  PlaYUVerMdiArea( QWidget *parent ) :
          QMdiArea( parent ),
          m_pixmapLogo( ":/images/playuver-backgroud-logo.png" )
  {
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
  m_uiWindowMode = 0;

  setWindowMode( MdiWSubWindows );

  m_pcActiveWindow = NULL;
  m_apcSubWindowList.clear();
  m_apcMdiSubWindowList.clear();
}

Void PlaYUVerSubWindowHandle::resetWindowMode()
{
  if( m_uiWindowMode == MdiWSubWindows )
  {
    removeAllSubWindow();
    m_apcMdiSubWindowList.clear();
    if( m_pcMdiArea )
      delete m_pcMdiArea;
    if( m_pcWindowManagerLayout )
      delete m_pcWindowManagerLayout;
  }
}

Void PlaYUVerSubWindowHandle::setWindowMode( UInt uiWindowMode )
{
  if( m_uiWindowMode == uiWindowMode )
    return;

  resetWindowMode();

  if( uiWindowMode == NormalSubWindows )
  {

  }

  if( uiWindowMode == MdiWSubWindows )
  {
    m_pcMdiArea = new PlaYUVerMdiArea( this );
    m_pcMdiArea->setActivationOrder( QMdiArea::ActivationHistoryOrder );
    connect( m_pcMdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( updateActiveSubWindow() ) );
    m_pcWindowManagerLayout = new QHBoxLayout( this );
    m_pcWindowManagerLayout->addWidget( m_pcMdiArea );
    setLayout( m_pcWindowManagerLayout );
  }

  m_uiWindowMode = uiWindowMode;
}

Void PlaYUVerSubWindowHandle::updateActiveSubWindow( SubWindowHandle *window )
{
  if( m_uiWindowMode == NormalSubWindows )
  {
    if( window )
      m_pcActiveWindow = window;
  }

  if( m_uiWindowMode == MdiWSubWindows )
  {
    m_pcActiveWindow = NULL;
    QMdiSubWindow* mdiSubWindow = m_pcMdiArea->activeSubWindow();
    if( mdiSubWindow )
    {
      QWidget* activeWidget = m_pcMdiArea->activeSubWindow()->widget();
      Int windowIdx = m_apcSubWindowList.indexOf( qobject_cast<SubWindowHandle *>( activeWidget ) );
      if( windowIdx >= 0 )
      {
        m_pcActiveWindow = m_apcSubWindowList.at( windowIdx );
        emit changed();
      }
    }
  }
}

Void PlaYUVerSubWindowHandle::addSubWindow( SubWindowHandle *window, Qt::WindowFlags flags )
{
  if( window )
  {
    connect( window, SIGNAL( updateStatusBar( const QString& ) ), qobject_cast<plaYUVerApp*>( parent() ), SLOT( updateStatusBar( const QString& ) ) );
    connect( window, SIGNAL( zoomFactorChanged_SWindow( const double, const QPoint ) ), qobject_cast<plaYUVerApp*>( parent() ), SLOT( updateZoomFactorSBox() ) );
    if( m_uiWindowMode == NormalSubWindows )
    {
      connect( window, SIGNAL( aboutToActivate( SubWindowHandle* ) ), this, SLOT( updateActiveSubWindow( SubWindowHandle* ) ) );
      connect( window, SIGNAL( aboutToClose( SubWindowHandle* ) ), this, SLOT( removeSubWindow( SubWindowHandle* ) ) );
    }
    if( m_uiWindowMode == MdiWSubWindows )
    {
      PlaYUVerMdiSubWindow* mdiSubWindow = new PlaYUVerMdiSubWindow;
      mdiSubWindow->setWidget( window );
      m_pcMdiArea->addSubWindow( mdiSubWindow );
      m_apcMdiSubWindowList.append( mdiSubWindow );
      window->setSubWindow( mdiSubWindow );
      connect( mdiSubWindow, SIGNAL( aboutToClose( PlaYUVerMdiSubWindow* ) ), this, SLOT( removeMdiSubWindow( PlaYUVerMdiSubWindow* ) ) );
    }
    m_apcSubWindowList.append( window );
  }
}

Void PlaYUVerSubWindowHandle::removeSubWindow( Int windowIdx )
{
  if( windowIdx >= 0 )
  {
    if( m_uiWindowMode == NormalSubWindows )
    {
      SubWindowHandle* subWindow = m_apcSubWindowList.at( windowIdx );
      m_apcSubWindowList.removeAt( windowIdx );
      subWindow->close();
    }
    if( m_uiWindowMode == MdiWSubWindows )
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

Void PlaYUVerSubWindowHandle::removeSubWindow( SubWindowHandle* window )
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

SubWindowHandle* PlaYUVerSubWindowHandle::activeSubWindow() const
{
  return m_pcActiveWindow;
}

QList<SubWindowHandle*> PlaYUVerSubWindowHandle::findSubWindow( const QString &windowName ) const
{
  QList<SubWindowHandle*> apcSubWindowList;
  if( windowName.isEmpty() )
  {
    apcSubWindowList = m_apcSubWindowList;
  }
  else
  {
    for( Int i = 0; i < m_apcSubWindowList.size(); i++ )
    {
      if( m_apcSubWindowList.at( i )->getWindowName() == windowName )
        apcSubWindowList.append( m_apcSubWindowList.at( i ) );
    }
  }
  return apcSubWindowList;
}

QList<SubWindowHandle*> PlaYUVerSubWindowHandle::findSubWindow( const UInt uiCategory ) const
{
  QList<SubWindowHandle*> apcSubWindowList;
  QList<SubWindowHandle*> subWindowList = findSubWindow();
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    if( subWindowList.at( i )->getCategory() & uiCategory )
      apcSubWindowList.append( subWindowList.at( i ) );
  }
  return apcSubWindowList;
}

SubWindowHandle* PlaYUVerSubWindowHandle::findSubWindow( const SubWindowHandle* subWindow ) const
{
  QList<SubWindowHandle*> subWindowList = findSubWindow();
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

  m_pcActiveWindow = qobject_cast<SubWindowHandle *>( window );
  m_pcActiveWindow->window()->activateWindow();
}

Void PlaYUVerSubWindowHandle::tileSubWindows()
{
  if( m_uiWindowMode == MdiWSubWindows )
  {
    m_pcMdiArea->tileSubWindows();
  }
}

Void PlaYUVerSubWindowHandle::cascadeSubWindows()
{
  if( m_uiWindowMode == MdiWSubWindows )
  {
    m_pcMdiArea->cascadeSubWindows();
  }
}

Void PlaYUVerSubWindowHandle::createActions()
{
  m_mapperWindow = new QSignalMapper( this );
  connect( m_mapperWindow, SIGNAL( mapped(QWidget*) ), this, SLOT( setActiveSubWindow(QWidget*) ) );

  m_arrayActions.resize( TOTAL_ACT );

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
    SubWindowHandle *subWindow = m_apcSubWindowList.at( i );

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

}

Void PlaYUVerSubWindowHandle::writeSettings()
{

}

}  // NAMESPACE
