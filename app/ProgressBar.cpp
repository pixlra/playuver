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
 * \file     ProgressBar.cpp
 * \brief    Progress bar code
 */

#include "ProgressBar.h"
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QDesktopWidget>

namespace plaYUVer
{

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

  QSize screenSize = QApplication::desktop()->availableGeometry().size();
  QPoint screenCenter = QApplication::desktop()->availableGeometry().center();
  resize( screenSize.width() / 4, screenSize.height() / 10 );
  move( screenCenter.x() - screenSize.width() / 8, screenCenter.y() - screenSize.height() / 10 );
  show();
}

Void ProgressBar::incrementProgress( UInt increment )
{
  m_uiProgress += increment;
  m_pcProgressBar->setValue( m_uiProgress );
}
}  // NAMESPACE
