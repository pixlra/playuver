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
 * \file     VideoSubWindow.cpp
 * \brief    Video Sub windows handling
 */

#include "VideoSubWindow.h"
#include "SubWindowHandle.h"
#include "ConfigureFormatDialog.h"
#include "ViewArea.h"

namespace plaYUVer
{

QDataStream& operator<<( QDataStream& out, const PlaYUVerStreamInfoVector& array )
{
  PlaYUVerStreamInfo d;
  out << array.size();
  for( Int i = 0; i < array.size(); i++ )
  {
    d = array.at( i );
    out << d.m_cFilename << d.m_uiWidth
        << d.m_uiHeight
        << d.m_iPelFormat
        << d.m_uiFrameRate;

  }
  return out;
}

QDataStream& operator>>( QDataStream& in, PlaYUVerStreamInfoVector& array )
{
  PlaYUVerStreamInfo d;
  Int array_size;
  in >> array_size;
  for( Int i = 0; i < array_size; i++ )
  {
    in >> d.m_cFilename;
    in >> d.m_uiWidth;
    in >> d.m_uiHeight;
    in >> d.m_iPelFormat;
    in >> d.m_uiFrameRate;
    array.append( d );
  }
  return in;
}

Int findPlaYUVerStreamInfo( PlaYUVerStreamInfoVector array, QString filename )
{
  for( Int i = 0; i < array.size(); i++ )
    if( array.at( i ).m_cFilename == filename )
      return i;
  return -1;
}

VideoSubWindow::VideoSubWindow( QWidget * parent, Bool isModule ) :
            SubWindowHandle( parent, SubWindowHandle::VIDEO_SUBWINDOW ),
            m_pCurrStream( NULL ),
            m_pcCurrFrame( NULL ),
            m_pcReferenceSubWindow( NULL ),
            m_bIsPlaying( false ),
            m_bIsModule( isModule )
{
  setVisible( false );

  // Create a new interface to show images
  m_cViewArea = new ViewArea( this );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SLOT( adjustScrollBarByScale( double, QPoint ) ) );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SIGNAL( zoomFactorChanged_SWindow( double, QPoint ) ) );

  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SLOT( adjustScrollBarByOffset( QPoint ) ) );
  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SIGNAL( scrollBarMoved_SWindow( QPoint ) ) );

  connect( m_cViewArea, SIGNAL( selectionChanged( QRect ) ), this, SLOT( updateSelectedArea( QRect ) ) );
  connect( m_cViewArea, SIGNAL( positionChanged( const QPoint & ) ), this, SLOT( updatePixelValueStatusBar( const QPoint & ) ) );

  // Define the cViewArea as the widget inside the scroll area
  setMainWidget( m_cViewArea );

  m_apcCurrentModule.clear();

}

VideoSubWindow::~VideoSubWindow()
{
  disableModule();
  delete m_cViewArea;
  if( m_pCurrStream )
    delete m_pCurrStream;
}

Void VideoSubWindow::reloadFile()
{
  Int currFrameNum = m_pCurrStream->getCurrFrameNum();
  loadFile( m_cFilename, false );
  seekAbsoluteEvent( currFrameNum );
}

Bool VideoSubWindow::loadFile( QString cFilename, Bool bForceDialog )
{
  UInt Width = 0, Height = 0, FrameRate = 30;
  Int InputFormat = PlaYUVerFrame::YUV420p;

  if( m_pCurrStream )
    m_pCurrStream->getFormat( Width, Height, InputFormat, FrameRate );

  if( m_pCurrStream->guessFormat( cFilename, Width, Height, InputFormat, FrameRate ) || bForceDialog )
  {
    ConfigureFormatDialog formatDialog( this );
    if( formatDialog.runConfigureFormatDialog( QFileInfo( cFilename ).fileName(), Width, Height, InputFormat, FrameRate ) == QDialog::Rejected )
    {
      return false;
    }
  }
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QApplication::restoreOverrideCursor();

  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }
  else
  {
    delete m_pCurrStream;
    m_pCurrStream = new PlaYUVerStream;
  }

  m_pCurrStream->open( cFilename, Width, Height, InputFormat, FrameRate );

  m_sStreamInfo.m_cFilename = cFilename;
  m_sStreamInfo.m_uiWidth = Width;
  m_sStreamInfo.m_uiHeight = Height;
  m_sStreamInfo.m_iPelFormat = InputFormat;
  m_sStreamInfo.m_uiFrameRate = FrameRate;

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = cFilename;
  m_cWindowShortName = QFileInfo( cFilename ).fileName();
  //setWindowName( m_pCurrStream->getStreamInformationString() );
  setWindowName( m_cWindowShortName );
  return true;
}

Bool VideoSubWindow::loadFile( PlaYUVerStreamInfo* streamInfo )
{
  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }

  if( !m_pCurrStream->open( streamInfo->m_cFilename, streamInfo->m_uiWidth, streamInfo->m_uiHeight, streamInfo->m_iPelFormat, streamInfo->m_uiFrameRate ) )
  {
    return false;
  }

  m_sStreamInfo = *streamInfo;

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = streamInfo->m_cFilename;
  m_cWindowShortName = QFileInfo( streamInfo->m_cFilename ).fileName();
  //setWindowName( m_pCurrStream->getStreamInformationString() );
  setWindowName( m_cWindowShortName );
  return true;
}

Void VideoSubWindow::updateSelectedArea( QRect area )
{
  m_cSelectedArea = area;
}

/**
 * Functions to enable a module in the
 * current SubWindow
 */
Void VideoSubWindow::enableModule( PlaYUVerAppModuleIf* pcModule, Bool bThisWindow )
{
  if( bThisWindow )
  {
    m_apcCurrentModule.clear();
    disableModule();
  }
  m_apcCurrentModule.append( pcModule );
}

Void VideoSubWindow::disableModule( PlaYUVerAppModuleIf* pcModule )
{
  if( pcModule )
  {
    Int modIdx = m_apcCurrentModule.indexOf( pcModule );
    if( modIdx != -1 )
    {
      m_apcCurrentModule.removeAt( modIdx );
      ModulesHandle::destroyModuleIf( pcModule );
    }
  }
  else
  {
    QList<PlaYUVerAppModuleIf*> apcCurrentModule = m_apcCurrentModule;
    for( Int i = 0; i < apcCurrentModule.size(); i++ )
    {
      m_apcCurrentModule.removeOne( apcCurrentModule.at( i ) );
      ModulesHandle::destroyModuleIf( apcCurrentModule.at( i ) );
    }
    assert( m_apcCurrentModule.size() == 0 );
  }
  refreshFrame();
}

Void VideoSubWindow::setCurrFrame( PlaYUVerFrame* pcCurrFrame )
{
  m_pcCurrFrame = pcCurrFrame;
  m_cViewArea->setImage( m_pcCurrFrame );
}

Void VideoSubWindow::refreshFrame()
{
  Bool bSetFrame = false;
  if( m_pCurrStream )
  {
    m_pcCurrFrame = m_pCurrStream->getCurrFrame();
    bSetFrame = true;
  }
  for( Int i = 0; i < m_apcCurrentModule.size(); i++ )
  {
    bSetFrame = ModulesHandle::applyModuleIf( m_apcCurrentModule.at( i ), m_bIsPlaying );
  }
  if( bSetFrame )
  {
    m_cViewArea->setImage( m_pcCurrFrame );
  }
}

Bool VideoSubWindow::save( QString filename )
{
  Bool iRet = false;
  QApplication::setOverrideCursor( Qt::WaitCursor );

  PlaYUVerFrame* saveFrame = m_pcCurrFrame;
  if( m_cSelectedArea.isValid() )
  {
    saveFrame = new PlaYUVerFrame( m_pcCurrFrame, m_cSelectedArea.x(), m_cSelectedArea.y(), m_cSelectedArea.width(), m_cSelectedArea.height() );
  }
  if( !saveFrame )
  {
    return false;
  }
  iRet = PlaYUVerStream::saveFrame( filename, saveFrame );
  QApplication::restoreOverrideCursor();
  return iRet;
}

Bool VideoSubWindow::play()
{
  if( m_pCurrStream && m_pCurrStream->getFrameNum() > 1 )
  {
    m_bIsPlaying = true;
  }
  else
  {
    m_bIsPlaying = false;
  }
  return m_bIsPlaying;
}

Bool VideoSubWindow::playEvent()
{
  if( m_pCurrStream && m_bIsPlaying )
  {
    m_pCurrStream->setNextFrame();
    refreshFrame();
    if( m_pCurrStream->checkErrors( PlaYUVerStream::END_OF_SEQ ) )
    {
      return true;
    }
    m_pCurrStream->readFrame();
  }
  return false;
}

Void VideoSubWindow::pause()
{
  m_bIsPlaying = false;
  refreshFrame();
}

Void VideoSubWindow::seekAbsoluteEvent( UInt new_frame_num )
{
  if( m_pCurrStream )
  {
    if( m_pCurrStream->seekInput( new_frame_num ) )
    {
      refreshFrame();
    }
  }
}

Void VideoSubWindow::seekRelativeEvent( Bool bIsFoward )
{
  if( m_pCurrStream )
  {
    if( m_pCurrStream->seekInput( m_pCurrStream->getCurrFrameNum() + ( bIsFoward ? 1 : -1 ) ) )
    {
      refreshFrame();
    }
  }
}

Void VideoSubWindow::stop()
{
  m_bIsPlaying = false;
  seekAbsoluteEvent( 0 );
  return;
}

Void VideoSubWindow::normalSize()
{
  Double factor = 1.0;
  Double curFactor = getScaleFactor() ;
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor/curFactor, QPoint() );
    adjustScrollBarByScale( usedScale, QPoint() );
  }
}

Void VideoSubWindow::zoomToFit()
{
  // Scale to a smaller size that the real to a nicer look
  scaleView( getScrollSize() );
}

Void VideoSubWindow::zoomToFactor( Double factor , QPoint center )
{
  Double curFactor;
  curFactor = getScaleFactor() ;
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor/curFactor, center );
    adjustScrollBarByScale( usedScale, center );
  }
}

Void VideoSubWindow::scaleView( Double scale , QPoint center )
{
  Q_ASSERT( m_cViewArea->image() );
  if(scale!=1.0)
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( scale, center );
    adjustScrollBarByScale( usedScale, center );
  }
}

Void VideoSubWindow::scaleView( const QSize & size, QPoint center )
{
  QSize imgViewSize;
  if( m_pcCurrFrame )
    imgViewSize = QSize( m_pcCurrFrame->getWidth(), m_pcCurrFrame->getHeight() );
  else
    imgViewSize = QSize( m_pCurrStream->getWidth(), m_pCurrStream->getHeight() );
  QSize newSize = imgViewSize;
  newSize.scale( size, Qt::KeepAspectRatio );

  // Calc the zoom factor
  Double wfactor = 1;
  Double hfactor = 1;
  Double factor;

  wfactor = ( Double )newSize.width() / imgViewSize.width();
  hfactor = ( Double )newSize.height() / imgViewSize.height();

  if( wfactor < hfactor )
    factor = wfactor;
  else
    factor = hfactor;

  Double curFactor = getScaleFactor() ;
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor/curFactor, center );
    adjustScrollBarByScale( usedScale, center );
  }
}

Void VideoSubWindow::updatePixelValueStatusBar( const QPoint& pos )
{
  if( m_pcCurrFrame )
  {
    PlaYUVerFrame::Pixel sPixelValue;
    Int iWidth, iHeight;
    Int posX = pos.x();
    Int posY = pos.y();
    QString strStatus;

    iWidth = m_pcCurrFrame->getWidth();
    iHeight = m_pcCurrFrame->getHeight();

    if( ( posX < iWidth ) && ( posX >= 0 ) && ( posY < iHeight ) && ( posY >= 0 ) )
    {
      strStatus = QString( "(%1,%2)   " ).arg( posX ).arg( posY );

      Int ColorSpace = m_pcCurrFrame->getColorSpace();
      if( ColorSpace == PlaYUVerFrame::COLOR_GRAY )
      {
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y(), PlaYUVerFrame::COLOR_YUV );
        strStatus.append( QString( "Y: %1" ).arg( sPixelValue.Y() ) );
      }
      if( ColorSpace == PlaYUVerFrame::COLOR_YUV )
      {
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y(), PlaYUVerFrame::COLOR_YUV );
        strStatus.append( QString( "Y: %1   U: %2   V: %3" ).arg( sPixelValue.Y() ).arg( sPixelValue.Cb() ).arg( sPixelValue.Cr() ) );
      }

      if( ( ColorSpace == PlaYUVerFrame::COLOR_RGB ) )
      {
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y(), PlaYUVerFrame::COLOR_YUV );
        strStatus.append( QString( "R: %1   G: %2   B: %3" ).arg( sPixelValue.R() ).arg( sPixelValue.G() ).arg( sPixelValue.B() ) );
      }
      emit updateStatusBar( strStatus );
    }
  }
}


QSize VideoSubWindow::sizeHint() const
{
  QSize maxSize;  // The size of the parent (viewport widget
  // of the QMdiArea).

  QWidget *p = parentWidget();
  if( p )
  {
    maxSize = p->size();
  }

  QSize isize;
  if( m_pcCurrFrame )
    isize = QSize( m_pcCurrFrame->getWidth() + 50, m_pcCurrFrame->getHeight() + 50 );
  else if( m_pCurrStream )
    isize = QSize( m_pCurrStream->getWidth() + 50, m_pCurrStream->getHeight() + 50 );

  // If the VideoSubWindow needs more space that the avaiable, we'll give
  // to the subwindow a reasonable size preserving the image aspect ratio.
  if( isize.width() < maxSize.width() && isize.height() < maxSize.height() )
  {
    return isize;
  }
  else
  {
    isize.scale( maxSize, Qt::KeepAspectRatio );
    maxSize = isize;
  }

  return maxSize;
}

Void VideoSubWindow::closeEvent( QCloseEvent *event )
{
  Bool bAccept = m_bIsPlaying ? false : true;
  if( bAccept )
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

}  // NAMESPACE
