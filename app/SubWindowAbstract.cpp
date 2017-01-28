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
 * \file     SubWindowAbstract.cpp
 * \brief    Abstract class to handle subwindows
 */

#include <QFocusEvent>
#include <QCloseEvent>
#include <QScrollBar>
#include <QIcon>
#include <QHBoxLayout>
#include "SubWindowAbstract.h"
#include "PlaYUVerMdiSubWindow.h"

SubWindowAbstract::SubWindowAbstract( QWidget* parent, UInt category ) : QWidget( parent ), m_cSubWindow( NULL )
{
  setParent( parent );
  setVisible( false );
  // setFocusPolicy( Qt::WheelFocus );
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Background );

  setWindowIcon( QIcon( ":/images/playuver.png" ) );

  // setWidgetResizable( true );
  m_pcLayout = NULL;
  m_uiCategory = category;
  m_cWindowName = QString( " " );

  connect( this, SIGNAL( destroyed() ), this, SLOT( onDestroyed() ) );
}

SubWindowAbstract::~SubWindowAbstract()
{
  return;
}

Void SubWindowAbstract::setWidget( QWidget* widget )
{
  QGridLayout* layout = new QGridLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( widget, 0, 0 );
}

void SubWindowAbstract::onDestroyed()
{
  return;
}

Void SubWindowAbstract::setWindowName( QString name )
{
  m_cWindowName = name;
  setWindowTitle( m_cWindowName );
}

QString SubWindowAbstract::getWindowName()
{
  return m_cWindowName;
}

Bool SubWindowAbstract::mayClose()
{
  return true;
}

QSize SubWindowAbstract::sizeHint() const
{
  return QSize();
}

QSize SubWindowAbstract::sizeHint( const QSize& ) const
{
  return QSize();
}

Void SubWindowAbstract::focusInEvent( QFocusEvent* event )
{
  emit aboutToActivate( this );
}

Void SubWindowAbstract::closeEvent( QCloseEvent* event )
{
  emit aboutToClose( this );
  event->accept();
}

Void SubWindowAbstract::closeSubWindow()
{
  close();
  if( m_cSubWindow )
    m_cSubWindow->close();
}
