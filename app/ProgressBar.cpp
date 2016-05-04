/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     ProgressBar.cpp
 * \brief    Progress bar code
 */

#include "ProgressBar.h"
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QDesktopWidget>

ProgressBar::ProgressBar( QWidget *parent, UInt maxValue ) :
        QWidget( parent ),
        m_uiTotalProgress( maxValue ),
        m_uiProgress( 0 )
{
  QVBoxLayout* mainLayout = new QVBoxLayout( this );
  m_pcProgressBar = new QProgressBar;
  m_pcProgressBar->setMaximum( m_uiTotalProgress );
  m_pcProgressBar->setAlignment( Qt::AlignCenter );
  mainLayout->addWidget( m_pcProgressBar );
  setLayout( mainLayout );
  setWindowTitle( QStringLiteral( "Progress" ) );

//  QSize appSize = QApplication::desktop()->availableGeometry().size();
//  QPoint appCenter = QApplication::desktop()->availableGeometry().center();
  QSize appSize = parent->size();
  QPoint appPos = parent->pos();
  QPoint appCenter = QPoint( appPos.x() + appSize.width() / 2, appPos.y() + appSize.height() / 2 );
  resize( appSize.width() / 4, appSize.height() / 10 );
  move( appCenter.x() - appSize.width() / 8, appCenter.y() - appSize.height() / 20 );
  show();
}

Void ProgressBar::incrementProgress( UInt increment )
{
  m_uiProgress += increment;
  m_pcProgressBar->setValue( m_uiProgress );
  update();
}

