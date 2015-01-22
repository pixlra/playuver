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
 * \file     PlaYUVerSubWinManager.cpp
 * \brief
 */

#include "PlaYUVerSubWinManager.h"
#include "PlaYUVerMdiSubWindow.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

class PlaYUVerMdiArea: public QMdiArea
{
public:
  PlaYUVerMdiArea( QWidget *parent = 0 ) :
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

PlaYUVerSubWinManager::PlaYUVerSubWinManager( QWidget *parent ) :
        QWidget( parent )
{
  m_bMdiModeEnabled = true;

  m_pcMdiArea = new PlaYUVerMdiArea( this );
  m_pcMdiArea->setActivationOrder( QMdiArea::ActivationHistoryOrder );

  QHBoxLayout* m_pcWindowManagerLayout = new QHBoxLayout( this );
  m_pcWindowManagerLayout->addWidget( m_pcMdiArea );
  setLayout( m_pcWindowManagerLayout );

  connect( m_pcMdiArea, SIGNAL( subWindowActivated(QMdiSubWindow*) ), this, SLOT( updateActiveSubWindow() ) );

  m_pcActiveWindow = NULL;
  m_apcSubWindowList.clear();
  m_apcMdiSubWindowList.clear();
}

Void PlaYUVerSubWinManager::updateActiveSubWindow()
{
  if( m_bMdiModeEnabled )
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
        emit windowActivated();
      }
    }
  }
}

Void PlaYUVerSubWinManager::addSubWindow( SubWindowHandle *window, Qt::WindowFlags flags )
{
  if( m_bMdiModeEnabled )
  {
    PlaYUVerMdiSubWindow* mdiSubWindow = new PlaYUVerMdiSubWindow( this );
    mdiSubWindow->setWidget( window );
    m_pcMdiArea->addSubWindow( mdiSubWindow );
    m_apcMdiSubWindowList.append( mdiSubWindow );
    connect( mdiSubWindow, SIGNAL( aboutToClose( PlaYUVerMdiSubWindow* ) ), this, SLOT( removeMdiSubWindow( PlaYUVerMdiSubWindow* ) ) );
  }
  m_apcSubWindowList.append( window );
}

Void PlaYUVerSubWinManager::removeSubWindow( Int windowIdx )
{
  if( windowIdx >= 0 )
  {
    if( m_bMdiModeEnabled )
    {
      m_pcMdiArea->removeSubWindow( m_apcSubWindowList.at( windowIdx ) );
      m_apcMdiSubWindowList.at( windowIdx )->close();
      m_apcMdiSubWindowList.removeAt( windowIdx );
    }
    m_apcSubWindowList.removeAt( windowIdx );
  }
}

Void PlaYUVerSubWinManager::removeSubWindow( SubWindowHandle* window )
{
  removeSubWindow( m_apcSubWindowList.indexOf( window ) );
}

Void PlaYUVerSubWinManager::removeMdiSubWindow( PlaYUVerMdiSubWindow* window )
{
  Int windowIdx = m_apcMdiSubWindowList.indexOf( window );
  if( windowIdx >= 0 )
  {

    m_apcMdiSubWindowList.removeAt( windowIdx );
    m_apcSubWindowList.removeAt( windowIdx );
  }
}

Void PlaYUVerSubWinManager::removeAllSubWindow()
{
  while( m_apcSubWindowList.size() > 0 )
  {
    removeSubWindow( 0 );
  }
}

SubWindowHandle* PlaYUVerSubWinManager::activeSubWindow() const
{
  return m_pcActiveWindow;
}

QList<SubWindowHandle*> PlaYUVerSubWinManager::findSubWindow( const QString &windowName ) const
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

QList<SubWindowHandle*> PlaYUVerSubWinManager::findSubWindow( const UInt uiCategory ) const
{
  QList<SubWindowHandle*> apcSubWindowList;
  QList<SubWindowHandle*> subWindowList = findSubWindow();
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    if( subWindowList.at( i )->getCategory() == uiCategory )
      apcSubWindowList.append( subWindowList.at( i ) );
  }
  return apcSubWindowList;
}

SubWindowHandle* PlaYUVerSubWinManager::findSubWindow( const SubWindowHandle* subWindow )
{
  QList<SubWindowHandle*> subWindowList = findSubWindow();
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    if( subWindow == subWindowList.at( i ) )
      return subWindowList.at( i );
  }
  return 0;
}

Void PlaYUVerSubWinManager::setActiveSubWindow( QWidget *window )
{
  if( !window )
    return;

  m_pcActiveWindow = qobject_cast<SubWindowHandle *>( window );
  window->font();
}

Void PlaYUVerSubWinManager::tileSubWindows()
{
  if( m_bMdiModeEnabled )
  {
    m_pcMdiArea->tileSubWindows();
  }
}

Void PlaYUVerSubWinManager::cascadeSubWindows()
{
  if( m_bMdiModeEnabled )
  {
    m_pcMdiArea->cascadeSubWindows();
  }
}

}  // NAMESPACE
