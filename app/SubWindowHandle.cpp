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

namespace plaYUVer
{

SubWindowHandle::SubWindowHandle( QWidget * parent, UInt category ) :
        QScrollArea( parent )
{
  setParent( parent );
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );

  // Create a new scroll area inside the sub-window
  connect( horizontalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );
  connect( verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );

  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  setWidgetResizable( true );

  m_uiCategory = category;
  m_cLastScroll = QPoint();
  m_cWindowName = QString( " " );
}

SubWindowHandle::~SubWindowHandle()
{
}

QSize SubWindowHandle::getScrollSize()
{
  return QSize( viewport()->size().width() - 5, viewport()->size().height() - 5 );
}

Void SubWindowHandle::adjustScrollBarByOffset( QPoint Offset )
{
  QScrollBar *scrollBar = horizontalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.x() ) );
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = verticalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.y() ) );
  m_cLastScroll.setY( scrollBar->value() );
  updateLastScrollValue();
}

// This function was developed with help of the schematics presented in
// http://stackoverflow.com/questions/13155382/jscrollpane-zoom-relative-to-mouse-position
Void SubWindowHandle::adjustScrollBarByScale( Double scale, QPoint center )
{
  QScrollBar *scrollBar = horizontalScrollBar();
  if( center.isNull() )
  {
    scrollBar->setValue( int( scale * scrollBar->value() + ( ( scale - 1 ) * scrollBar->pageStep() / 2 ) ) );
  }
  else
  {
    Int x = center.x() - m_cLastScroll.x();
    Int value = int( scale * m_cLastScroll.x() + ( ( scale - 1 ) * x ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    scrollBar->setValue( value );
  }

  scrollBar = verticalScrollBar();
  if( center.isNull() )
  {
    scrollBar->setValue( int( scale * scrollBar->value() + ( ( scale - 1 ) * scrollBar->pageStep() / 2 ) ) );
  }
  else
  {
    Int y = center.y() - m_cLastScroll.y();
    Int value = int( scale * m_cLastScroll.y() + ( ( scale - 1 ) * y ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    scrollBar->setValue( value );
  }

  updateLastScrollValue();
}

Void SubWindowHandle::updateLastScrollValue()
{
  QScrollBar *scrollBar = horizontalScrollBar();
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = verticalScrollBar();
  m_cLastScroll.setY( scrollBar->value() );
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

Void SubWindowHandle::focusInEvent( QFocusEvent * event )
{
  emit aboutToActivate( this );
}

Void SubWindowHandle::closeEvent( QCloseEvent *event )
{
  emit aboutToClose( this );
  event->accept();
}

}  // NAMESPACE
