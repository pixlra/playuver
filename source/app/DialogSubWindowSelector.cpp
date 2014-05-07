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
 * \file     DialogSubWindowSelector.cpp
 * \brief    Dialog box to select sub windows
 */

#include "DialogSubWindowSelector.h"
#include "PlaYUVerFrame.h"
#include "InputStream.h"

namespace plaYUVer
{

DialogSubWindowSelector::DialogSubWindowSelector( QWidget *parent, QMdiArea *mdiArea ) :
        QDialog( parent )
{
  QFont titleFont, menusFont;

  titleFont.setPointSize( 14 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  menusFont.setWeight( 75 );

  resize( 392, 100 );
  setWindowTitle( "Sub Windows Selection" );

  QVBoxLayout* MainLayout = new QVBoxLayout( this );

  // Confirmation buttons
  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );


  // Add components
  MainLayout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  MainLayout->addWidget( dialogButtonOkCancel );

  // Create actions
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
  QMetaObject::connectSlotsByName( this );

  // Set variables
  m_pcMainWindowMdiArea = mdiArea;

}

}  // Namespace SCode
