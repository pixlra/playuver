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
        QMdiSubWindow( parent )
{
  setParent( parent );
  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );

  // Create a new scroll area inside the sub-window
  m_cScrollArea = new QScrollArea( this );
  connect( m_cScrollArea->horizontalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );
  connect( m_cScrollArea->verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );

  setWidget( m_cScrollArea );
  m_cScrollArea->setWidgetResizable( true );

  m_cLastScroll = QPoint();

  m_uiCategory = category;
  m_cWindowName = QString( " " );
}

SubWindowHandle::~SubWindowHandle()
{
  delete m_cScrollArea;
}

Void SubWindowHandle::setMainWidget( QWidget *widget )
{
  m_cScrollArea->setWidget( widget );
}

QSize SubWindowHandle::getScrollSize()
{
  return QSize(  m_cScrollArea->viewport()->size().width() - 5, m_cScrollArea->viewport()->size().height() - 5 );
}

Void SubWindowHandle::adjustScrollBarByOffset( QPoint Offset )
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.x() ) );
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = m_cScrollArea->verticalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.y() ) );
  m_cLastScroll.setY( scrollBar->value() );
  updateLastScrollValue();
}

// This function was developed with help of the schematics presented in
// http://stackoverflow.com/questions/13155382/jscrollpane-zoom-relative-to-mouse-position
Void SubWindowHandle::adjustScrollBarByScale( Double scale, QPoint center )
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
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

  scrollBar = m_cScrollArea->verticalScrollBar();
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
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = m_cScrollArea->verticalScrollBar();
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

}  // NAMESPACE
