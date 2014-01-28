/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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

/** \file     ConfigureResolutionDialog.cpp
 \brief    Dialog box to set the sequence resolution
 */
#include <QApplication>
#include "ConfigureResolutionDialog.h"

namespace plaYUVer
{

ConfigureResolutionDialog::ConfigureResolutionDialog( QWidget *parent ) :
    QDialog( parent )
{

  setStandardResolutionSizes();

  if( objectName().isEmpty() )
    setObjectName( QStringLiteral( "ConfigureResolution" ) );

  // Config dialog

  setObjectName( QStringLiteral( "ConfigureResolution" ) );
  resize( 392, 292 );

  setWindowTitle( QApplication::translate( "ConfigureResolution", "Configure Resolution", 0 ) );
  setWindowIcon( QIcon( ":/images/dialogicon-grid.png" ) );

  MainLayout = new QVBoxLayout( this );
  MainLayout->setObjectName( QStringLiteral( "MainLayout" ) );

  // headLayout

  headLayout = new QHBoxLayout( this );
  headLayout->setObjectName( QStringLiteral( "headLayout" ) );

  dialogTitleLabel = new QLabel( this );
  dialogTitleLabel->setObjectName( QStringLiteral( "dialogTitleLabel" ) );
  QFont font;
  font.setPointSize( 11 );
  font.setBold( true );
  font.setWeight( 75 );
  dialogTitleLabel->setFont( font );
  dialogTitleLabel->setText( QApplication::translate( "ConfigureResolution", "Configure Resolution", 0 ) );

  headerSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  dialogIconLabel = new QLabel( this );
  dialogIconLabel->setObjectName( QStringLiteral( "dialogIconLabel" ) );
  dialogIconLabel->setPixmap( QPixmap( ":/images/dialogheader-grid.png" ) );

  headLayout->addWidget( dialogTitleLabel );
  headLayout->addItem( headerSpacer );
  headLayout->addWidget( dialogIconLabel );

  MainLayout->addLayout( headLayout );

  // standardResolutionLayout

  standardResolutionLayout = new QHBoxLayout( this );
  standardResolutionLayout->setObjectName( QStringLiteral( "standardResolutionLayout" ) );
  standardResolutionLabel = new QLabel( this );
  standardResolutionLabel->setObjectName( QStringLiteral( "standardResolutionLabel" ) );

  horizontalSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  standardResolutionBox = new QComboBox( this );
  standardResolutionBox->setObjectName( QStringLiteral( "standardResolutionBox" ) );
  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( standardResolutionBox->sizePolicy().hasHeightForWidth() );
  standardResolutionBox->setSizePolicy( sizePolicy );
  standardResolutionBox->setAcceptDrops( true );

  standardResolutionLayout->addWidget( standardResolutionLabel );
  standardResolutionLayout->addItem( horizontalSpacer );
  standardResolutionLayout->addWidget( standardResolutionBox );
  standardResolutionLabel->setText( QApplication::translate( "ConfigureResolution", "Standard Resolution", 0 ) );
  standardResolutionBox->clear();
  standardResolutionBox->insertItems( 0, standardResolutionNames );

  MainLayout->addLayout( standardResolutionLayout );

  // resolutionGrid

  resolutionGrid = new QGridLayout( this );
  resolutionGrid->setObjectName( QStringLiteral( "resolutionGrid" ) );

  resolutionLabel = new QLabel( this );
  resolutionLabel->setObjectName( QStringLiteral( "resolutionLabel" ) );
  QFont font1;
  font1.setBold( true );
  font1.setWeight( 75 );
  resolutionLabel->setFont( font1 );
  resolutionLabel->setText( QApplication::translate( "ConfigureResolution", "Resolution chosen", 0 ) );

  widthLabel = new QLabel( this );
  widthLabel->setObjectName( QStringLiteral( "widthLabel" ) );
  widthLabel->setText( QApplication::translate( "ConfigureResolution", "Width", 0 ) );
  widthSpinBox = new QSpinBox( this );
  widthSpinBox->setObjectName( QStringLiteral( "widthSpinBox" ) );
  widthSpinBox->setRange( 0, 5000 );
  widthSpinBox->setValue( 0 );

  heightLabel = new QLabel( this );
  heightLabel->setObjectName( QStringLiteral( "heightLabel" ) );
  heightLabel->setText( QApplication::translate( "ConfigureResolution", "Height", 0 ) );
  heightSpinBox = new QSpinBox( this );
  heightSpinBox->setObjectName( QStringLiteral( "heightSpinBox" ) );
  heightSpinBox->setRange( 0, 5000 );
  heightSpinBox->setValue( 0 );

  pixelsLabel = new QLabel( this );
  pixelsLabel->setObjectName( QStringLiteral( "pixelsLabel" ) );
  pixelsLabel->setText( QApplication::translate( "ConfigureResolution", "Pixels", 0 ) );

  resolutionGrid->addWidget( resolutionLabel, 0, 0, 1, 1 );
  resolutionGrid->addWidget( widthLabel, 1, 1, 1, 1 );
  resolutionGrid->addWidget( widthSpinBox, 2, 1, 1, 1 );
  resolutionGrid->addWidget( heightLabel, 1, 2, 1, 1 );
  resolutionGrid->addWidget( heightSpinBox, 2, 2, 1, 1 );
  resolutionGrid->addWidget( pixelsLabel, 2, 3, 1, 1 );

  MainLayout->addLayout( resolutionGrid );

  verticalSpacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
  MainLayout->addItem( verticalSpacer );

  // Confirmation buttons

  dialogButtonOkCancel = new QDialogButtonBox( this );
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  MainLayout->addWidget( dialogButtonOkCancel );

  connect( standardResolutionBox, SIGNAL( currentIndexChanged() ), this, SLOT( StandardResolutionSelection() ) );
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );

}


void ConfigureResolutionDialog::StandardResolutionSelection()
{
  Int currIdx = standardResolutionBox->currentIndex();
  if( currIdx != 1 )
  {
    widthSpinBox->setValue( 0 );
    heightSpinBox->setValue( 0 );
  }
}

} // Namespace SCode
