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
#include "SubWindowHandle.h"
//#include "PlaYUVerFrame.h"
//#include "InputStream.h"

namespace plaYUVer
{

DialogSubWindowSelector::DialogSubWindowSelector( QWidget *parent, QMdiArea *mdiArea ) :
        QDialog( parent )
{
  QFont titleFont, menusFont;

  // Set variables
  m_pcMainWindowMdiArea = mdiArea;

  titleFont.setPointSize( 14 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  menusFont.setWeight( 75 );

  resize( 392, 100 );
  setWindowTitle( "Sub Windows Selection" );

  QVBoxLayout* MainLayout = new QVBoxLayout( this );

  // Window list and add button
  m_comboBoxWindowList = new QComboBox();
  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( m_comboBoxWindowList->sizePolicy().hasHeightForWidth() );
  m_comboBoxWindowList->setSizePolicy( sizePolicy );
  m_comboBoxWindowList->setAcceptDrops( true );

  updateSubWindowList();

  m_pushButtonAdd = new QPushButton("Add", this);

  // Selected Sub Window List
  m_listSelectedWindows = new QListWidget( this );

  // Confirmation buttons
  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  // Create Layouts
  QHBoxLayout* headLayout = new QHBoxLayout();
  headLayout->addWidget( m_comboBoxWindowList );
  headLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  headLayout->addWidget( m_pushButtonAdd );

  // Add components
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  MainLayout->addLayout( headLayout );
  MainLayout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  MainLayout->addWidget( m_listSelectedWindows );
  MainLayout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  MainLayout->addWidget( dialogButtonOkCancel );

  setLayout( MainLayout );

  // Create actions
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
  connect( m_pushButtonAdd, SIGNAL( clicked() ), this, SLOT( addSubWindow() ) );
  QMetaObject::connectSlotsByName( this );

}

Void DialogSubWindowSelector::updateSubWindowList()
{
  SubWindowHandle *subWindow;
  QString currSubWindowName;
  for( Int i = 0; i < m_pcMainWindowMdiArea->subWindowList().size(); i++ )
  {
    subWindow = qobject_cast<SubWindowHandle *>( m_pcMainWindowMdiArea->subWindowList().at( i ) );
    currSubWindowName = subWindow->getWindowName();
    //if( m_listSelectedWindows->contai )
    m_pcWindowListNames.append( currSubWindowName );
  }
  m_comboBoxWindowList->clear();
  m_comboBoxWindowList->insertItems( 0, m_pcWindowListNames );
  m_comboBoxWindowList->setCurrentIndex( -1 );
}

void DialogSubWindowSelector::addSubWindow()
{

}

}  // Namespace SCode
