/*    This file is a part of PlaYUVer project
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
 * \file     SeekStreamDialog.cpp
 * \brief    Dialog box to config modules opts
 */

#include "SeekStreamDialog.h"

#include "lib/PlaYUVerStream.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVector>
#include <QWidget>

SeekStreamDialog::SeekStreamDialog( PlaYUVerStream* pcCurrStream, QWidget* parent )
    : QDialog( parent, Qt::Dialog | Qt::WindowTitleHint ), m_iSelectedFrameNum()
{
  m_iSelectedFrameNum = pcCurrStream->getCurrFrameNum();

  setWindowModality( Qt::ApplicationModal );
  setWindowTitle( "Seek Video" );

  QFont labelFont;
  QLabel* pcLabel = new QLabel;
  pcLabel->setText( QString( "Go to frame (0 - %1)" ).arg( pcCurrStream->getFrameNum() - 1 ) );

  m_spinFrameNum = new QSpinBox;
  m_spinFrameNum->setMinimumSize( 70, 5 );
  m_spinFrameNum->setMaximumSize( 70, 30 );
  m_spinFrameNum->setMinimum( 0 );
  m_spinFrameNum->setMaximum( pcCurrStream->getFrameNum() - 1 );
  m_spinFrameNum->setValue( m_iSelectedFrameNum );

  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  QVBoxLayout* verticalLayout = new QVBoxLayout;
  QHBoxLayout* horizontalLayout = new QHBoxLayout;
  horizontalLayout->addWidget( pcLabel );
  horizontalLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  horizontalLayout->addWidget( m_spinFrameNum );
  verticalLayout->addLayout( horizontalLayout );
  verticalLayout->addWidget( dialogButtonOkCancel, Qt::AlignRight );
  setLayout( verticalLayout );

  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
}

Int SeekStreamDialog::runDialog()
{
  m_spinFrameNum->selectAll();
  if( exec() == QDialog::Rejected )
  {
    return -1;
  }
  m_iSelectedFrameNum = m_spinFrameNum->value();
  return m_iSelectedFrameNum;
}
