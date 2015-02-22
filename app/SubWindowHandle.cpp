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
 * \file     SubWindowHandle.cpp
 * \brief    Sub windows handling
 */

#include "SubWindowHandle.h"
#include "PlaYUVerMdiSubWindow.h"
namespace plaYUVer
{

SubWindowHandle::SubWindowHandle( QWidget * parent, UInt category ) :
        QScrollArea( parent ),
        m_cSubWindow( NULL )
{
  setParent( parent );
  setFocusPolicy( Qt::WheelFocus );
  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Background );

  setWindowIcon( QIcon( ":/images/playuver.png" ) );

  // Create a new scroll area inside the sub-window
  connect( horizontalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateCurScrollValues() ) );
  connect( verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateCurScrollValues() ) );

  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  setWidgetResizable( true );

  m_uiCategory = category;
  m_cLastScroll = QPoint();
  m_cWindowName = QString( " " );

  connect(this, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
}

void SubWindowHandle::onDestroyed()
{
  return;
}

SubWindowHandle::~SubWindowHandle()
{
  return;
}

QSize SubWindowHandle::getScrollSize()
{
  return QSize( viewport()->size().width() - 5, viewport()->size().height() - 5 );
}

Void SubWindowHandle::adjustScrollBarByOffset( QPoint Offset )
{
  QPoint cLastScroll = m_cCurrScroll;
  m_cCurrScroll.setX(int( cLastScroll.x() + Offset.x() ));
  m_cCurrScroll.setY(int( cLastScroll.y() + Offset.y() ));

  // Update window scroll
  QScrollBar *scrollBar = horizontalScrollBar();
  scrollBar->setValue( m_cCurrScroll.x() );
  scrollBar = verticalScrollBar();
  scrollBar->setValue( m_cCurrScroll.y() );
}

// This function was developed with help of the schematics presented in
// http://stackoverflow.com/questions/13155382/jscrollpane-zoom-relative-to-mouse-position
Void SubWindowHandle::adjustScrollBarByScale( Double scale, QPoint center )
{
  QPoint cLastScroll = m_cCurrScroll;
  QScrollBar *scrollBar = horizontalScrollBar();
  if( center.isNull() )
  {
    m_cCurrScroll.setX(int( scale * cLastScroll.x() + ( ( scale - 1 ) * scrollBar->pageStep() / 2 ) ));
  }
  else
  {
    Int x = center.x() - cLastScroll.x();
    Int value = int( scale * cLastScroll.x() + ( ( scale - 1 ) * x ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    m_cCurrScroll.setX(value);
  }

  scrollBar = verticalScrollBar();
  if( center.isNull() )
  {
    m_cCurrScroll.setY(int( scale * cLastScroll.y() + ( ( scale - 1 ) * scrollBar->pageStep() / 2 ) ));
  }
  else
  {
    Int y = center.y() - cLastScroll.y();
    Int value = int( scale * cLastScroll.y() + ( ( scale - 1 ) * y ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    m_cCurrScroll.setY(value);
  }

  // Update window scroll
  scrollBar = horizontalScrollBar();
  scrollBar->setValue( m_cCurrScroll.x() );
  scrollBar = verticalScrollBar();
  scrollBar->setValue( m_cCurrScroll.y() );

}

Void SubWindowHandle::updateCurScrollValues()
{
  QScrollBar *scrollBar = horizontalScrollBar();
  m_cCurrScroll.setX( scrollBar->value() );
  scrollBar = verticalScrollBar();
  m_cCurrScroll.setY( scrollBar->value() );
}

Void SubWindowHandle::setCurScrollValues()
{
  QScrollBar *scrollBar = horizontalScrollBar();
  scrollBar->setValue(m_cCurrScroll.x() );
  scrollBar = verticalScrollBar();
  scrollBar->setValue(m_cCurrScroll.y() );
}

Void SubWindowHandle::setWindowName( QString name )
{
  m_cWindowName = name;
  setWindowTitle( m_cWindowName );
}

QString SubWindowHandle::getWindowName()
{
  return m_cWindowName;
}

Bool SubWindowHandle::mayClose()
{
  return true;
}

QSize SubWindowHandle::sizeHint( const QSize & ) const
{
  return QSize();
}

Void SubWindowHandle::focusInEvent( QFocusEvent * event )
{
  emit aboutToActivate( this );
}

Void SubWindowHandle::closeEvent( QCloseEvent *event )
{
  emit aboutToClose( this );
  event->accept();
}

Void SubWindowHandle::closeSubWindow()
{
  close();
  if( m_cSubWindow )
    m_cSubWindow->close();
}

}  // NAMESPACE
