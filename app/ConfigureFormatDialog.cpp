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
 * \file     ConfigureFormatDialog.cpp
 * \brief    Dialog box to set the sequence resolution
 */

#include "ConfigureFormatDialog.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"

namespace plaYUVer
{

ConfigureFormatDialog::ConfigureFormatDialog( QWidget *parent ) :
        QDialog( parent )
{
  QString Name;
  UInt uiWidth, uiHeight;
  std::vector<PlaYUVerStdResolution> listPlaYUVerStdResolution = PlaYUVerStream::stdResolutionSizes();
  for( UInt i = 0; i < listPlaYUVerStdResolution.size(); i++ )
  {
    Name = QString::fromStdString( listPlaYUVerStdResolution[i].shortName );
    uiWidth = listPlaYUVerStdResolution[i].uiWidth;
    uiHeight = listPlaYUVerStdResolution[i].uiHeight;
    standardResolutionNames.append( QString( "%1 (%2x%3)" ).arg( Name ).arg( uiWidth ).arg( uiHeight ) );
    standardResolutionSizes.append( QSize( uiWidth, uiHeight ) );
  }

  setObjectName( "ConfigureFormat" );

  setWindowTitle( "Configure Resolution" );
  setWindowIcon( QIcon( ":/images/configureformat.png" ) );

  QVBoxLayout* MainLayout = new QVBoxLayout( this );

  QFont titleFont, menusFont, normalFont;

  titleFont.setPointSize( 12 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  titleFont.setWeight( 75 );

  normalFont.setBold( false );
  // menusFont.setPointSize( 12 );

//  // headLayout
//  QHBoxLayout* headLayout = new QHBoxLayout();
//  QLabel* dialogTitleLabel = new QLabel();
//  dialogTitleLabel->setFont( titleFont );
//  dialogTitleLabel->setText( "Configure Resolution" );
//  headLayout->addWidget( dialogTitleLabel );
//  headLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
//
//  MainLayout->addLayout( headLayout );
//  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );

  /*
   *  Filename layout
   */
  QHBoxLayout* filenameLayout = new QHBoxLayout();
  QLabel* filenameLabel = new QLabel();
  filenameLabel->setText( "Name" );
  filenameLabel->setFont( menusFont );
  m_labelFilename = new QLabel();
  m_labelFilename->setFont( titleFont );
  filenameLayout->addWidget( filenameLabel );
  filenameLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  filenameLayout->addWidget( m_labelFilename );
  MainLayout->addLayout( filenameLayout );

  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );

  /*
   *  Resolution
   */
  QGroupBox* resolutionGroup = new QGroupBox();
  resolutionGroup->setTitle( QStringLiteral( "Resolution" ) );
  resolutionGroup->setFont( menusFont );

  QGridLayout* resolutionGrid = new QGridLayout();
  resolutionGroup->setLayout( resolutionGrid );

  m_comboBoxStandardResolution = new QComboBox();

  m_comboBoxStandardResolution->setFont( normalFont );
  m_comboBoxStandardResolution->setSizePolicy( sizePolicy );
  m_comboBoxStandardResolution->setAcceptDrops( true );
  m_comboBoxStandardResolution->clear();
  m_comboBoxStandardResolution->insertItems( 0, standardResolutionNames );
  m_comboBoxStandardResolution->setCurrentIndex( -1 );

  QLabel* widthLabel = new QLabel( "Width" );
  widthLabel->setFont( normalFont );
  m_spinBoxWidth = new QSpinBox();
  m_spinBoxWidth->setRange( 0, 9999 );
  m_spinBoxWidth->setValue( 0 );
  m_spinBoxWidth->setFont( normalFont );

  QLabel* heightLabel = new QLabel( "Height" );
  heightLabel->setFont( normalFont );
  m_spinBoxheight = new QSpinBox();
  m_spinBoxheight->setRange( 0, 9999 );
  m_spinBoxheight->setValue( 0 );
  m_spinBoxheight->setFont( normalFont );

  QLabel* pixelsLabel = new QLabel();
  pixelsLabel->setText( "Pixels" );
  pixelsLabel->setFont( normalFont );
  resolutionGrid->addWidget( m_comboBoxStandardResolution, 1, 0, Qt::AlignCenter );
  resolutionGrid->addWidget( widthLabel, 0, 1, Qt::AlignCenter );
  resolutionGrid->addWidget( m_spinBoxWidth, 1, 1, Qt::AlignCenter );
  resolutionGrid->addWidget( heightLabel, 0, 2, Qt::AlignCenter );
  resolutionGrid->addWidget( m_spinBoxheight, 1, 2, Qt::AlignCenter );
  resolutionGrid->addWidget( pixelsLabel, 1, 3, Qt::AlignCenter );

  /*
   *  Pixel format
   */
  QGroupBox* pixelGroup = new QGroupBox();
  pixelGroup->setTitle( QStringLiteral( "Pixel Format" ) );
  pixelGroup->setFont( menusFont );
  QGridLayout* pixelGridLayout = new QGridLayout();
  pixelGroup->setLayout( pixelGridLayout );

  QLabel* colorSpaceLabel = new QLabel();
  colorSpaceLabel->setText( "Color Space" );
  colorSpaceLabel->setFont( normalFont );
  m_comboBoxColorSpace = new QComboBox();
  m_comboBoxColorSpace->setSizePolicy( sizePolicy );
  m_comboBoxColorSpace->setFont( normalFont );
  m_comboBoxColorSpace->clear();

  QLabel* pixelFormatLabel = new QLabel();
  pixelFormatLabel->setText( "Sampling" );
  pixelFormatLabel->setFont( normalFont );
  m_comboBoxPixelFormat = new QComboBox();
  m_comboBoxPixelFormat->setSizePolicy( sizePolicy );
  m_comboBoxPixelFormat->setFont( normalFont );
  m_comboBoxPixelFormat->clear();

  for( UInt i = 0; i < PlaYUVerFrame::supportedColorSpacesListNames().size(); i++ )
  {
    m_comboBoxColorSpace->insertItem( i, PlaYUVerFrame::supportedColorSpacesListNames()[i].c_str() );
  }
  m_comboBoxColorSpace->setCurrentIndex( 1 );

  pixelGridLayout->addWidget( colorSpaceLabel, 0, 0 );
  pixelGridLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  pixelGridLayout->addWidget( m_comboBoxColorSpace, 0, 2 );

  pixelGridLayout->addWidget( pixelFormatLabel, 1, 0 );
  pixelGridLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ), 1, 1 );
  pixelGridLayout->addWidget( m_comboBoxPixelFormat, 1, 2 );

  /*
   *  Frame rate format
   */
  QHBoxLayout* bitsPerPelLayout = new QHBoxLayout();
  QLabel* bitsPerPelLabel = new QLabel();
  bitsPerPelLabel->setFont( menusFont );
  bitsPerPelLabel->setText( "Frame Rate" );
  m_spinBoxBits = new QSpinBox();
  m_spinBoxBits->setMinimumWidth( 30 );
  m_spinBoxBits->setRange( 0, 16 );
  m_spinBoxBits->setValue( 8 );
  bitsPerPelLayout->addWidget( bitsPerPelLabel );
  bitsPerPelLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  bitsPerPelLayout->addWidget( m_spinBoxBits );

  /*
   *  Frame rate format
   */
  QHBoxLayout* framerateFormatLayout = new QHBoxLayout();
  QLabel* framerateFormatLabel = new QLabel();
  framerateFormatLabel->setFont( menusFont );
  framerateFormatLabel->setText( "Frame Rate" );
  m_spinBoxFrameRate = new QSpinBox();
  m_spinBoxFrameRate->setSuffix( QStringLiteral( " Hz" ) );
  m_spinBoxFrameRate->setMinimumWidth( 30 );
  m_spinBoxFrameRate->setRange( 0, 200 );
  m_spinBoxFrameRate->setValue( 30 );
  framerateFormatLayout->addWidget( framerateFormatLabel );
  framerateFormatLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  framerateFormatLayout->addWidget( m_spinBoxFrameRate );

  /*
   *  Confirmation buttons
   */
  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  // MainLayout->addLayout( standardResolutionLayout );
  // MainLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Minimum ) );
  MainLayout->addWidget( resolutionGroup );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  MainLayout->addWidget( pixelGroup );
  MainLayout->addLayout( bitsPerPelLayout );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  MainLayout->addLayout( framerateFormatLayout );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  MainLayout->addWidget( dialogButtonOkCancel );

  connect( m_comboBoxStandardResolution, SIGNAL( currentIndexChanged(int) ), this, SLOT( StandardResolutionSelection() ) );
  connect( m_comboBoxColorSpace, SIGNAL( currentIndexChanged(int) ), this, SLOT( slotColorSpaceChange(int) ) );
  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );

  m_comboBoxColorSpace->setCurrentIndex( 0 );

  setFixedSize( MainLayout->sizeHint() );

}

Int ConfigureFormatDialog::runConfigureFormatDialog( QString Filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rBits, UInt& rFrameRate )
{
  // Set default values
  //setWindowTitle( "Configure resolution for " + Filename );
  m_labelFilename->setText( QFileInfo( Filename ).fileName() );
  m_spinBoxWidth->setValue( rWidth );
  m_spinBoxheight->setValue( rHeight );

  Int colorSpace = 0;
  Int sampling = 0;
  if( rInputFormat >= 0 )
  {
    std::string pelFmtName = PlaYUVerFrame::supportedPixelFormatListNames()[rInputFormat];
    for( UInt j = 0; j < PlaYUVerFrame::supportedColorSpacesListNames().size(); j++ )
    {
      for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames( j ).size(); i++ )
      {
        if( pelFmtName == PlaYUVerFrame::supportedPixelFormatListNames( j )[i] )
        {
          colorSpace = j;
          sampling = i;
          break;
        }
      }
    }
  }
  m_comboBoxColorSpace->setCurrentIndex( colorSpace );
  m_comboBoxPixelFormat->setCurrentIndex( sampling );
  m_spinBoxBits->setValue( rBits );
  m_spinBoxFrameRate->setValue( rFrameRate );
  for( Int i = 0; i < standardResolutionSizes.size(); i++ )
  {
    if( standardResolutionSizes.at( i ) == QSize( rWidth, rHeight ) )
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
  colorSpace = m_comboBoxColorSpace->currentIndex();
  sampling = m_comboBoxPixelFormat->currentIndex();
  std::string pelFmtName = PlaYUVerFrame::supportedPixelFormatListNames( colorSpace )[sampling];
  for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames().size(); i++ )
  {
    if( pelFmtName == PlaYUVerFrame::supportedPixelFormatListNames()[i] )
    {
      rInputFormat = i;
    }
  }
  rBits = m_spinBoxBits->value();
  rFrameRate = m_spinBoxFrameRate->value();
  return QDialog::Accepted;
}

void ConfigureFormatDialog::StandardResolutionSelection()
{
  Int currIdx = m_comboBoxStandardResolution->currentIndex();
  if( currIdx == -1 )
    return;

  QSize currSize = standardResolutionSizes.at( currIdx );
  m_spinBoxWidth->setValue( currSize.width() );
  m_spinBoxheight->setValue( currSize.height() );
}

Void ConfigureFormatDialog::slotColorSpaceChange( Int idx )
{
  if( idx == -1 )
    return;

  m_comboBoxPixelFormat->clear();
  for( UInt i = 0; i < PlaYUVerFrame::supportedPixelFormatListNames( idx ).size(); i++ )
  {
    m_comboBoxPixelFormat->insertItem( i, PlaYUVerFrame::supportedPixelFormatListNames( idx )[i].c_str() );
  }
  m_comboBoxPixelFormat->setCurrentIndex( 0 );
}

}  // NAMESPACE
