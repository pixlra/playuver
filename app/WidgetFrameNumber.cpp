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
 * \file     WidgetFrameNumber.h
 * \brief    Frame number widget
 */

#include "config.h"
#include <QtCore>
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include "WidgetFrameNumber.h"

namespace plaYUVer
{

WidgetFrameNumber::WidgetFrameNumber( QWidget *parent ) :
        QWidget( parent )
{
  QHBoxLayout* mainlayout = new QHBoxLayout;
  m_pcCurrFrameNumLabel = new QLabel;
  m_pcCurrFrameNumLabel->setText( "-" );
  m_pcCurrFrameNumLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  m_pcCurrFrameNumLabel->setMinimumWidth( 25 );
  m_pcCurrFrameNumLabel->setAlignment( Qt::AlignRight );
  QLabel *forwardslash = new QLabel;
  forwardslash->setText( "/" );
  m_pcTotalFrameNumLabel = new QLabel;
  m_pcTotalFrameNumLabel->setText( "-" );
  m_pcTotalFrameNumLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  m_pcTotalFrameNumLabel->setMinimumWidth( 30 );
  m_pcTotalFrameNumLabel->setAlignment( Qt::AlignLeft );
  mainlayout->addWidget( m_pcCurrFrameNumLabel );
  mainlayout->addWidget( forwardslash );
  mainlayout->addWidget( m_pcTotalFrameNumLabel );
  setLayout( mainlayout );
}
Void WidgetFrameNumber::setCurrFrameNum( Int number )
{
  m_pcCurrFrameNumLabel->setText( QString( tr( "%1" ) ).arg( number ) );
}
Void WidgetFrameNumber::setTotalFrameNum( Int number )
{
  m_pcTotalFrameNumLabel->setText( QString( tr( "%1" ) ).arg( number ) );
}
Void WidgetFrameNumber::clear()
{
  m_pcCurrFrameNumLabel->setText( "-" );
  m_pcTotalFrameNumLabel->setText( "-" );
}

}  // NAMESPACE

