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
#include "VideoSubWindow.h"

namespace plaYUVer
{

DialogSubWindowSelector::DialogSubWindowSelector( QWidget *parent, QMdiArea *mdiArea, Int minWindowsSelected, Int maxWindowsSelected ) :
        QDialog( parent ),
        m_iMinSelectedWindows( minWindowsSelected ),
        m_iMaxSlectedWindows( maxWindowsSelected ),
        m_pcMainWindowMdiArea( mdiArea )
{
  QFont titleFont, menusFont;

  titleFont.setPointSize( 14 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  menusFont.setWeight( 75 );

  QSize windowSize( 350, 80 );
  resize( windowSize );
  setWindowTitle( "Sub Windows Selection" );

  QHBoxLayout* MainLayout = new QHBoxLayout( this );

  // Window list and add button
  m_comboBoxWindowList = new QComboBox();
  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( m_comboBoxWindowList->sizePolicy().hasHeightForWidth() );
  m_comboBoxWindowList->setSizePolicy( sizePolicy );
  m_comboBoxWindowList->setAcceptDrops( true );

  m_pushButtonAdd = new QPushButton( "Add", this );
  m_pushButtonAddAll = new QPushButton( "Add all", this );
  m_pushButtonRemove = new QPushButton( "Remove", this );
  m_pushButtonRemove->setEnabled( false );

  // Selected Sub Window List
  m_listSelectedWindows = new QListWidget( this );

  // Confirmation buttons
  m_pushButtonOkCancel = new QDialogButtonBox();
  m_pushButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  m_pushButtonOkCancel->setOrientation( Qt::Vertical );
  m_pushButtonOkCancel->setCenterButtons( false );
  m_pushButtonOkCancel->buttons().at( 0 )->setEnabled( false );

  // Create Layouts
  QVBoxLayout* LeftVLayout = new QVBoxLayout();
  LeftVLayout->addWidget( m_comboBoxWindowList );
  //LeftVLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  LeftVLayout->addWidget( m_listSelectedWindows );

  QVBoxLayout* RightVLayout = new QVBoxLayout();
  RightVLayout->addWidget( m_pushButtonAdd );
  RightVLayout->addWidget( m_pushButtonAddAll );
  RightVLayout->addWidget( m_pushButtonRemove );
  RightVLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  RightVLayout->addWidget( m_pushButtonOkCancel );

  // Add components
  //MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  MainLayout->addLayout( LeftVLayout );
  MainLayout->addLayout( RightVLayout );

  setLayout( MainLayout );

  // Create actions
  connect( m_pushButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( m_pushButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
  connect( m_pushButtonAdd, SIGNAL( clicked() ), this, SLOT( addSubWindow() ) );
  connect( m_pushButtonAddAll, SIGNAL( clicked() ), this, SLOT( addAllSubWindow() ) );
  connect( m_pushButtonRemove, SIGNAL( clicked() ), this, SLOT( removeSubWindow() ) );
  QMetaObject::connectSlotsByName( this );

  updateSubWindowList();

}

Void DialogSubWindowSelector::updateSubWindowList()
{
  SubWindowHandle *subWindow;
  VideoSubWindow *videoSubWindow;
  QString currSubWindowName;
  m_pcWindowListNames.clear();
  for( Int i = 0; i < m_pcMainWindowMdiArea->subWindowList().size(); i++ )
  {
    subWindow = qobject_cast<SubWindowHandle *>( m_pcMainWindowMdiArea->subWindowList().at( i ) );
    if( subWindow->getCategory() == SubWindowHandle::VIDEO_SUBWINDOW )
    {
      videoSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowMdiArea->subWindowList().at( i ) );
      currSubWindowName = videoSubWindow->getWindowName();
      if( !m_pcSelectedWindowListNames.contains( currSubWindowName ) && !videoSubWindow->getIsModule() )
        m_pcWindowListNames.append( currSubWindowName );
    }
  }
  m_comboBoxWindowList->clear();
  m_comboBoxWindowList->insertItems( 0, m_pcWindowListNames );
  m_comboBoxWindowList->setCurrentIndex( -1 );

  Bool enableAddAll = true;

  if( m_iMaxSlectedWindows >= 0 )
  {
    if( m_pcSelectedWindowListNames.size() >= m_iMaxSlectedWindows )
    {
      m_pushButtonAdd->setEnabled( false );
    }
    else
    {
      m_pushButtonAdd->setEnabled( true );
    }
    if( m_pcWindowListNames.size() > m_iMaxSlectedWindows )
    {
      enableAddAll = false;
    }
  }
  m_pushButtonAddAll->setEnabled( enableAddAll );

  if( m_pcSelectedWindowListNames.size() > 0 )
  {
    m_pushButtonRemove->setEnabled( true );
  }
  else
  {
    m_pushButtonRemove->setEnabled( false );
  }

  if( m_pcSelectedWindowListNames.size() >= m_iMinSelectedWindows )
  {
    m_pushButtonOkCancel->buttons().at( 0 )->setEnabled( true );
  }
  else
  {
    m_pushButtonOkCancel->buttons().at( 0 )->setEnabled( false );
  }

}

Void DialogSubWindowSelector::setSubWindowList( QStringList cWindowListNames )
{
  m_pcSelectedWindowListNames = cWindowListNames;
  m_listSelectedWindows->clear();
  m_listSelectedWindows->insertItems( 0, cWindowListNames );
  updateSubWindowList();
}

// -----------------------  Slot Functions  -----------------------

Void DialogSubWindowSelector::addSubWindow()
{
  Int iSelectedIdx = m_comboBoxWindowList->currentIndex();
  if( iSelectedIdx >= 0 )
  {
    m_pcSelectedWindowListNames.append( m_pcWindowListNames.at( iSelectedIdx ) );
    m_listSelectedWindows->clear();
    m_listSelectedWindows->insertItems( 0, m_pcSelectedWindowListNames );
    updateSubWindowList();
  }
}

Void DialogSubWindowSelector::addAllSubWindow()
{
  if( m_iMaxSlectedWindows <= 0 || m_pcWindowListNames.size() <= m_iMaxSlectedWindows )
  {
    for( Int i = 0; i < m_pcWindowListNames.size(); i++ )
    {
      m_pcSelectedWindowListNames.append( m_pcWindowListNames.at( i ) );
    }
    m_listSelectedWindows->clear();
    m_listSelectedWindows->insertItems( 0, m_pcSelectedWindowListNames );
    updateSubWindowList();
  }
}

Void DialogSubWindowSelector::removeSubWindow()
{
  QList<QListWidgetItem*> selectedWindows = m_listSelectedWindows->selectedItems();
  if( selectedWindows.size() )
  {
    for( Int i = 0; i < selectedWindows.size(); i++ )
    {
      QString currName = selectedWindows.at( i )->text();
      qDebug( ) << currName;
      Int indexCurrName = m_pcSelectedWindowListNames.indexOf( currName );
      qDebug( ) << indexCurrName;
      m_pcSelectedWindowListNames.removeAt( indexCurrName );
      m_listSelectedWindows->removeItemWidget( selectedWindows[i] );
    }
    m_listSelectedWindows->clear();
    m_listSelectedWindows->insertItems( 0, m_pcSelectedWindowListNames );
    updateSubWindowList();
  }

}

}  // Namespace SCode
