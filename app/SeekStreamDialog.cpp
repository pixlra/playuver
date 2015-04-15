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
 * \file     SeekStreamDialog.cpp
 * \brief    Dialog box to config modules opts
 */

#include "SeekStreamDialog.h"
#include "lib/PlaYUVerStream.h"
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QGridLayout>
#include <QSpinBox>

namespace plaYUVer
{

SeekStreamDialog::SeekStreamDialog( PlaYUVerStream *pcCurrStream, QWidget *parent ) :
        QDialog( parent ),
        m_iSelectedFrameNum()
{

  m_iSelectedFrameNum = pcCurrStream->getCurrFrameNum();

  setWindowTitle( "Go to frame" );

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

  QGridLayout* mainLayout = new QGridLayout;
  mainLayout->addWidget( pcLabel, 0, 0 );
  mainLayout->addWidget( m_spinFrameNum, 0, 1 );
  mainLayout->addWidget( dialogButtonOkCancel, 1, 0, 1, 2, Qt::AlignRight );
  setLayout( mainLayout );

  setFixedSize( 250, mainLayout->sizeHint().height() );

  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
}

Int SeekStreamDialog::runDialog()
{
  if( exec() == QDialog::Rejected )
  {
    return -1;
  }
  m_iSelectedFrameNum = m_spinFrameNum->value();
  return m_iSelectedFrameNum;
}

}  // NAMESPACE
