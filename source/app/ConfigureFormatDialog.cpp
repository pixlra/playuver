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
 * \file     ConfigureFormatDialog.cpp
 * \brief    Dialog box to set the sequence resolution
 */

#include "ConfigureFormatDialog.h"
#include "PlaYUVerFrame.h"
#include "InputStream.h"

namespace plaYUVer
{

ConfigureFormatDialog::ConfigureFormatDialog( QWidget *parent ) :
        QDialog( parent )
{
  QFont titleFont, menusFont;

  titleFont.setPointSize( 14 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  menusFont.setWeight( 75 );

  // Config dialog

  setStandardResolutionSizes();

  setObjectName( QStringLiteral( "ConfigureFormat" ) );
  resize( 392, 350 );

  setWindowTitle( "Configure Resolution" );
  setWindowIcon( QIcon( ":/images/configureformat.png" ) );

  QVBoxLayout* MainLayout = new QVBoxLayout( this );
  MainLayout->setObjectName( QStringLiteral( "MainLayout" ) );

  // headLayout
  QHBoxLayout* headLayout = new QHBoxLayout();
  QLabel* dialogTitleLabel = new QLabel();
  dialogTitleLabel->setFont( titleFont );
  dialogTitleLabel->setText( "Configure Resolution" );
  QLabel* dialogIconLabel = new QLabel();
  dialogIconLabel->setObjectName( QStringLiteral( "dialogIconLabel" ) );
  dialogIconLabel->setPixmap( QPixmap( ":/images/dialogheader-grid.png" ) );
  headLayout->addWidget( dialogTitleLabel );
  headLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  headLayout->addWidget( dialogIconLabel );

  MainLayout->addLayout( headLayout );

  // standardResolutionLayout
  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );
  QHBoxLayout* standardResolutionLayout = new QHBoxLayout();
  QLabel* standardResolutionLabel = new QLabel();
  standardResolutionLabel->setText( "Standard Resolution" );
  standardResolutionLabel->setFont( menusFont );
  m_comboBoxStandardResolution = new QComboBox();
  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );
  sizePolicy.setHeightForWidth( m_comboBoxStandardResolution->sizePolicy().hasHeightForWidth() );
  m_comboBoxStandardResolution->setSizePolicy( sizePolicy );
  m_comboBoxStandardResolution->setAcceptDrops( true );
  m_comboBoxStandardResolution->clear();
  m_comboBoxStandardResolution->insertItems( 0, standardResolutionNames );
  m_comboBoxStandardResolution->setCurrentIndex( -1 );
  standardResolutionLayout->addWidget( standardResolutionLabel );
  standardResolutionLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  standardResolutionLayout->addWidget( m_comboBoxStandardResolution );

  MainLayout->addLayout( standardResolutionLayout );

  // Resolution Grid
  //resolutionGridVerticalSpacer = new QSpacerItem( 10, 20, QSizePolicy::Minimum );
  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );

  QGridLayout* resolutionGrid = new QGridLayout();

  QLabel* resolutionLabel = new QLabel();
  resolutionLabel->setFont( menusFont );
  resolutionLabel->setText( "Resolution chosen" );
  QLabel* widthLabel = new QLabel( "Width" );
  m_spinBoxWidth = new QSpinBox();
  m_spinBoxWidth->setRange( 0, 7240 );
  m_spinBoxWidth->setValue( 0 );

  QLabel* heightLabel = new QLabel( "Height" );
  m_spinBoxheight = new QSpinBox();
  m_spinBoxheight->setRange( 0, 5432 );
  m_spinBoxheight->setValue( 0 );

  QLabel* pixelsLabel = new QLabel();
  pixelsLabel->setText( "Pixels" );

  resolutionGrid->addWidget( resolutionLabel, 0, 0, 1, 1 );
  resolutionGrid->addWidget( widthLabel, 1, 1, 1, 1 );
  resolutionGrid->addWidget( m_spinBoxWidth, 2, 1, 1, 1 );
  resolutionGrid->addWidget( heightLabel, 1, 2, 1, 1 );
  resolutionGrid->addWidget( m_spinBoxheight, 2, 2, 1, 1 );
  resolutionGrid->addWidget( pixelsLabel, 2, 3, 1, 1 );
  MainLayout->addLayout( resolutionGrid );

  // Pixel format
  QHBoxLayout* pixelFormatLayout = new QHBoxLayout();
  QLabel* pixelFormatLabel = new QLabel();
  pixelFormatLabel->setText( "Pixel Format" );
  pixelFormatLabel->setFont( menusFont );
  m_comboBoxPixelFormat = new QComboBox();
  QSizePolicy sizePolicyPixelFormat( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicyPixelFormat.setHorizontalStretch( 0 );
  sizePolicyPixelFormat.setVerticalStretch( 0 );
  sizePolicyPixelFormat.setHeightForWidth( m_comboBoxPixelFormat->sizePolicy().hasHeightForWidth() );
  m_comboBoxPixelFormat->setSizePolicy( sizePolicyPixelFormat );
  m_comboBoxPixelFormat->setAcceptDrops( true );
  m_comboBoxPixelFormat->clear();
  m_comboBoxPixelFormat->insertItems( 0, PlaYUVerFrame::supportedPixelFormatList() );
  m_comboBoxPixelFormat->setCurrentIndex( 1 );

  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );
  pixelFormatLayout->addWidget( pixelFormatLabel );
  pixelFormatLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  pixelFormatLayout->addWidget( m_comboBoxPixelFormat );
  MainLayout->addLayout( pixelFormatLayout );

  // Frame rate format
  QHBoxLayout* framerateFormatLayout = new QHBoxLayout();
  QLabel* framerateFormatLabel = new QLabel();
  framerateFormatLabel->setFont( menusFont );
  framerateFormatLabel->setText( "Frame Rate" );
  m_spinBoxFrameRate = new QSpinBox();
  m_spinBoxFrameRate->setRange( 0, 200 );
  m_spinBoxFrameRate->setValue( 30 );

  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );
  framerateFormatLayout->addWidget( framerateFormatLabel );
  framerateFormatLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  framerateFormatLayout->addWidget( m_spinBoxFrameRate );

  MainLayout->addLayout( framerateFormatLayout );

  // Confirmation buttons

  MainLayout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  MainLayout->addWidget( dialogButtonOkCancel );

  connect( m_comboBoxStandardResolution, SIGNAL( currentIndexChanged(int) ), this, SLOT( StandardResolutionSelection() ) );
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );

  QMetaObject::connectSlotsByName( this );

}

Int ConfigureFormatDialog::runConfigureFormatDialog( UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate )
{
  // Set default values
  m_spinBoxWidth->setValue( rWidth );
  m_spinBoxheight->setValue( rHeight );
  m_comboBoxPixelFormat->setCurrentIndex( rInputFormat >= 0 ? rInputFormat : 0 );
  m_spinBoxFrameRate->setValue( rFrameRate );
  for( Int i = 0; i < standardResolutionSizesList.size(); i++ )
  {
    if( standardResolutionSizesList.at( i ) == QSize( rWidth, rHeight ) )
    {
      m_comboBoxStandardResolution->setCurrentIndex( i );
    }
  }
  if( exec() == QDialog::Rejected )
  {
    return QDialog::Rejected;
  }
  rWidth = m_spinBoxWidth->value();
  rHeight = m_spinBoxheight->value();
  rInputFormat = m_comboBoxPixelFormat->currentIndex();
  rFrameRate = m_spinBoxFrameRate->value();
  return QDialog::Accepted;
}

void ConfigureFormatDialog::StandardResolutionSelection()
{
  Int currIdx = m_comboBoxStandardResolution->currentIndex();
  if( currIdx == -1 )
    return;

  QSize currSize = standardResolutionSizesList.at( currIdx );
  m_spinBoxWidth->setValue( currSize.width() );
  m_spinBoxheight->setValue( currSize.height() );
}

}  // Namespace SCode
