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

/**
 * \file     ConfigureFormatDialog.cpp
 * \brief    Dialog box to set the sequence resolution
 */

#include <QApplication>
#include <QComboBox>
#include "ConfigureFormatDialog.h"
#include "InputStream.h"

namespace plaYUVer
{

ConfigureFormatDialog::ConfigureFormatDialog( QWidget *parent ) :
    QDialog( parent )
{

  setStandardResolutionSizes();

  // Config dialog

  setObjectName( QStringLiteral( "ConfigureFormat" ) );
  resize( 392, 350 );

  setWindowTitle( QApplication::translate( "ConfigureFormat", "Configure Resolution", 0 ) );
  setWindowIcon( QIcon( ":/images/dialogicon-grid.png" ) );

  MainLayout = new QVBoxLayout( this );
  MainLayout->setObjectName( QStringLiteral( "MainLayout" ) );

  // headLayout

  headLayout = new QHBoxLayout();
  headLayout->setObjectName( QStringLiteral( "headLayout" ) );

  dialogTitleLabel = new QLabel( this );
  dialogTitleLabel->setObjectName( QStringLiteral( "dialogTitleLabel" ) );
  QFont font;
  font.setPointSize( 11 );
  font.setBold( true );
  font.setWeight( 75 );
  dialogTitleLabel->setFont( font );
  dialogTitleLabel->setText( QApplication::translate( "ConfigureFormat", "Configure Resolution", 0 ) );

  headerSpacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  dialogIconLabel = new QLabel( this );
  dialogIconLabel->setObjectName( QStringLiteral( "dialogIconLabel" ) );
  dialogIconLabel->setPixmap( QPixmap( ":/images/dialogheader-grid.png" ) );

  headLayout->addWidget( dialogTitleLabel );
  headLayout->addItem( headerSpacer );
  headLayout->addWidget( dialogIconLabel );

  MainLayout->addLayout( headLayout );

  // standardResolutionLayout

  verticalSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum );
  MainLayout->addItem( verticalSpacer );

  standardResolutionLayout = new QHBoxLayout( );
  standardResolutionLayout->setObjectName( QStringLiteral( "standardResolutionLayout" ) );
  standardResolutionLabel = new QLabel( this );
  standardResolutionLabel->setObjectName( QStringLiteral( "standardResolutionLabel" ) );
  standardResolutionLabel->setText( QApplication::translate( "ConfigureFormat", "Standard Resolution", 0 ) );

  horizontalSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  standardResolutionBox = new QComboBox( this );
  standardResolutionBox->setObjectName( QStringLiteral( "standardResolutionBox" ) );
  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( standardResolutionBox->sizePolicy().hasHeightForWidth() );
  standardResolutionBox->setSizePolicy( sizePolicy );
  standardResolutionBox->setAcceptDrops( true );

  standardResolutionBox->clear();
  standardResolutionBox->insertItems( 0, standardResolutionNames );
  standardResolutionBox->setCurrentIndex( -1 );

  standardResolutionLayout->addWidget( standardResolutionLabel );
  standardResolutionLayout->addItem( horizontalSpacer );
  standardResolutionLayout->addWidget( standardResolutionBox );

  MainLayout->addLayout( standardResolutionLayout );

  // resolutionGrid

  resolutionGridVerticalSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum );
  MainLayout->addItem( resolutionGridVerticalSpacer );

  resolutionGrid = new QGridLayout( this );
  resolutionGrid->setObjectName( QStringLiteral( "resolutionGrid" ) );

  resolutionLabel = new QLabel( this );
  resolutionLabel->setObjectName( QStringLiteral( "resolutionLabel" ) );
  QFont font1;
  font1.setBold( true );
  font1.setWeight( 75 );
  resolutionLabel->setFont( font1 );
  resolutionLabel->setText( QApplication::translate( "ConfigureFormat", "Resolution chosen", 0 ) );

  widthLabel = new QLabel( this );
  widthLabel->setObjectName( QStringLiteral( "widthLabel" ) );
  widthLabel->setText( QApplication::translate( "ConfigureFormat", "Width", 0 ) );
  widthSpinBox = new QSpinBox( this );
  widthSpinBox->setObjectName( QStringLiteral( "widthSpinBox" ) );
  widthSpinBox->setRange( 0, 5000 );
  widthSpinBox->setValue( 0 );

  heightLabel = new QLabel( this );
  heightLabel->setObjectName( QStringLiteral( "heightLabel" ) );
  heightLabel->setText( QApplication::translate( "ConfigureFormat", "Height", 0 ) );
  heightSpinBox = new QSpinBox( this );
  heightSpinBox->setObjectName( QStringLiteral( "heightSpinBox" ) );
  heightSpinBox->setRange( 0, 5000 );
  heightSpinBox->setValue( 0 );

  pixelsLabel = new QLabel( this );
  pixelsLabel->setObjectName( QStringLiteral( "pixelsLabel" ) );
  pixelsLabel->setText( QApplication::translate( "ConfigureFormat", "Pixels", 0 ) );

  resolutionGrid->addWidget( resolutionLabel, 0, 0, 1, 1 );
  resolutionGrid->addWidget( widthLabel, 1, 1, 1, 1 );
  resolutionGrid->addWidget( widthSpinBox, 2, 1, 1, 1 );
  resolutionGrid->addWidget( heightLabel, 1, 2, 1, 1 );
  resolutionGrid->addWidget( heightSpinBox, 2, 2, 1, 1 );
  resolutionGrid->addWidget( pixelsLabel, 2, 3, 1, 1 );

  MainLayout->addLayout( resolutionGrid );

  // Pixel format

  pixelFormatVerticalSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum );
  MainLayout->addItem( pixelFormatVerticalSpacer );

  pixelFormatLayout = new QHBoxLayout( );
  pixelFormatLayout->setObjectName( QStringLiteral( "pixelFormatLayout" ) );

  pixelFormatLabel = new QLabel( this );
  pixelFormatLabel->setObjectName( QStringLiteral( "pixelFormatLabel" ) );
  pixelFormatLabel->setText( QApplication::translate( "pixelFormat", "Pixel FormatBox", 0 ) );

  pixelFormatHorizontalSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

  pixelFormatBox = new QComboBox( this );
  pixelFormatBox->setObjectName( QStringLiteral( "pixelFormatBox" ) );
  QSizePolicy sizePolicyPixelFormat( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicyPixelFormat.setHorizontalStretch( 0 );
  sizePolicyPixelFormat.setVerticalStretch( 0 );
  sizePolicyPixelFormat.setHeightForWidth( pixelFormatBox->sizePolicy().hasHeightForWidth() );
  pixelFormatBox->setSizePolicy( sizePolicyPixelFormat );
  pixelFormatBox->setAcceptDrops( true );
  pixelFormatBox->clear();
  pixelFormatBox->insertItems( 0, InputStream::supportedPixelFormatList() );
  pixelFormatBox->setCurrentIndex( 0 );

  pixelFormatLayout->addWidget( pixelFormatLabel );
  pixelFormatLayout->addItem( pixelFormatHorizontalSpacer );
  pixelFormatLayout->addWidget( pixelFormatBox );

  MainLayout->addLayout( pixelFormatLayout );

  // Confirmation buttons

  verticalSpacerConfirmation = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
  MainLayout->addItem( verticalSpacerConfirmation );

  dialogButtonOkCancel = new QDialogButtonBox( this );
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  MainLayout->addWidget( dialogButtonOkCancel );

  Int idx;
  connect( standardResolutionBox, SIGNAL( currentIndexChanged(int) ), this, SLOT( StandardResolutionSelection() ) );
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );

  QMetaObject::connectSlotsByName(this);

}

void ConfigureFormatDialog::StandardResolutionSelection()
{
  Int currIdx = standardResolutionBox->currentIndex();

  if( currIdx == -1 )
    return;

  QList<QSize>::iterator iteratorSizes = standardResolutionSizesList.begin();

  for( Int i = 0; i < currIdx; i++ )
    iteratorSizes++;

  QSize currSize = ( QSize )( *iteratorSizes );

  widthSpinBox->setValue( currSize.width() );
  heightSpinBox->setValue( currSize.height() );

}

}  // Namespace SCode
