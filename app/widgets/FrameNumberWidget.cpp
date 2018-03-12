/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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

#include "FrameNumberWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

FrameNumberWidget::FrameNumberWidget( QWidget* parent )
    : QWidget( parent )
{
  QHBoxLayout* mainlayout = new QHBoxLayout;
  m_pcCurrFrameNumLabel = new QLabel;
  m_pcCurrFrameNumLabel->setText( "-" );
  m_pcCurrFrameNumLabel->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  m_pcCurrFrameNumLabel->setMinimumWidth( 25 );
  m_pcCurrFrameNumLabel->setAlignment( Qt::AlignRight );
  QLabel* forwardslash = new QLabel;
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
void FrameNumberWidget::setCurrFrameNum( int number )
{
  m_pcCurrFrameNumLabel->setText( QString( tr( "%1" ) ).arg( number ) );
}
void FrameNumberWidget::setTotalFrameNum( int number )
{
  m_pcTotalFrameNumLabel->setText( QString( tr( "%1" ) ).arg( number ) );
}
void FrameNumberWidget::clear()
{
  m_pcCurrFrameNumLabel->setText( "-" );
  m_pcTotalFrameNumLabel->setText( "-" );
}
