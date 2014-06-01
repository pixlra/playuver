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
 * \file     SubWindowHandle.cpp
 * \brief    Sub windows handling
 */

#include "ConfigureFormatDialog.h"
#include "SubWindowHandle.h"
#include "ViewArea.h"

namespace plaYUVer
{

SubWindowHandle::SubWindowHandle( QWidget * parent ) :
        QMdiSubWindow( parent )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );
  setVisible( false );

  // Create a new scroll area inside the sub-window
  m_cScrollArea = new QScrollArea( this );
  setWidget( m_cScrollArea );

  // Create a new interface to show images
  m_cViewArea = new ViewArea( m_cScrollArea );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged( double , QPoint) ), this, SLOT( adjustScrollBarByZoom(double, QPoint) ) );
  connect( m_cViewArea, SIGNAL( moveScroll( QPoint ) ), this, SLOT( adjustScrollBarByOffset(QPoint) ) );
  connect( m_cScrollArea->horizontalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );
  connect( m_cScrollArea->verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateLastScrollValue() ) );

  // Define the cViewArea as the widget inside the scroll area
  m_cScrollArea->setWidget( m_cViewArea );
  m_cScrollArea->setWidgetResizable( true );

  m_pCurrStream = NULL;
  //m_pCurrStream = new InputStream;
  m_pcCurrFrame = NULL;
  m_pcCurrentModule = NULL;

  m_cWindowName = QString( " " );
  m_bIsPlaying = false;
  m_cLastScroll = QPoint();
}

SubWindowHandle::~SubWindowHandle()
{
  disableModule();
  delete m_cViewArea;
  delete m_cScrollArea;
  if( m_pCurrStream )
    delete m_pCurrStream;
}

Bool SubWindowHandle::loadFile( const QString &fileName )
{
  UInt Width = 0, Height = 0, FrameRate = 30;
  Int InputFormat = -1;

  if( !m_pCurrStream )
  {
    m_pCurrStream = new InputStream;
  }

  m_pCurrStream->getFormat( Width, Height, InputFormat, FrameRate );

  if( m_pCurrStream->guessFormat( fileName, Width, Height, InputFormat, FrameRate ) )
  {
    ConfigureFormatDialog formatDialog( this );
    if( formatDialog.runConfigureFormatDialog( Width, Height, InputFormat, FrameRate ) == QDialog::Rejected )
    {
      return false;
    }
  }
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QApplication::restoreOverrideCursor();
  if( !m_pCurrStream->open( fileName, Width, Height, InputFormat, FrameRate ) )
  {
    return false;
  }

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cCurrFileName = fileName;
  m_cWindowName = m_pCurrStream->getStreamInformationString();
  setWindowTitle( m_cWindowName );
  return true;
}

/**
 * Functions to enable a module in the
 * current SubWindow
 */
Void SubWindowHandle::enableModule( PlaYUVerModuleIf* select_module )
{
  if( m_pcCurrentModule )
  {
    disableModule();
  }
  m_pcCurrentModule = select_module;
 // refreshFrame();
}

Void SubWindowHandle::disableModule()
{
  if( m_pcCurrentModule )
  {
    PlaYUVerModuleIf* pcCurrentModule = m_pcCurrentModule;
    m_pcCurrentModule = NULL;
    ModulesHandle::destroyModuleIf( pcCurrentModule );
    refreshFrame();
  }
}

Void SubWindowHandle::swapModuleFrames()
{
  if( m_pcCurrentModule )
  {
    ModulesHandle::swapModulesWindowsIf( m_pcCurrentModule );
    refreshFrame();
  }
}

Void SubWindowHandle::setCurrFrame( PlaYUVerFrame* pcCurrFrame )
{
  m_pcCurrFrame = pcCurrFrame;
  m_cViewArea->setImage( m_pcCurrFrame );
  if( !isVisible() )
  {
    show();
    resize( sizeHint() );
    zoomToFit();

  }
}

Void SubWindowHandle::refreshFrame()
{
  Bool bSetFrame = false;
  if( m_pCurrStream )
  {
    m_pcCurrFrame = m_pCurrStream->getCurrFrame();
    bSetFrame = true;
    if( m_pcCurrentModule )
    {
      bSetFrame = ModulesHandle::applyModuleIf( m_pcCurrentModule, m_bIsPlaying );
    }
  }
  else
  {
    if( m_pcCurrentModule )
    {
      bSetFrame = ModulesHandle::applyModuleIf( m_pcCurrentModule, m_bIsPlaying );
    }
  }
  if( bSetFrame )
  {
    m_cViewArea->setImage( m_pcCurrFrame );
  }
}

Bool SubWindowHandle::save()
{
  QString supported = tr( "Supported Files" );
  QString formats = InputStream::supportedWriteFormats();
  formats.prepend( " (" );
  formats.append( ")" );
  supported.append( formats );  // supported=="Supported Files (*.pbm *.jpg...)"
  QStringList filter;
  filter << supported
         << InputStream::supportedWriteFormatsList()
         << tr( "All Files (*)" );
  QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ), m_cCurrFileName, filter.join( ";;" ) );
  if( fileName.isEmpty() )
    return false;

  QApplication::setOverrideCursor( Qt::WaitCursor );
  m_pCurrStream->getCurrFrame()->FrametoRGB8();
  QImage qimg = QImage( m_pCurrStream->getCurrFrame()->getQImageBuffer(), m_pCurrStream->getCurrFrame()->getWidth(), m_pCurrStream->getCurrFrame()->getHeight(),
      QImage::Format_RGB888 );
  if( qimg.save( fileName ) )
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot save file %1" ).arg( fileName ) );
    return false;
  }
  QApplication::restoreOverrideCursor();
  return true;
}

Void SubWindowHandle::play()
{
  m_bIsPlaying = true;
}

Int SubWindowHandle::playEvent()
{
  if( m_pCurrStream && m_bIsPlaying )
  {
    m_pCurrStream->setNextFrame();
    refreshFrame();
    if( m_pCurrStream->checkErrors( InputStream::END_OF_SEQ ) )
    {
      return -2;
    }
    m_pCurrStream->readNextFrame();
    if( m_pCurrStream->checkErrors( InputStream::READING ) )
    {
      QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( m_cCurrFileName ) );
      return -3;
    }
    return 0;
  }
  return -4;
}

Void SubWindowHandle::pause()
{
  m_bIsPlaying = false;
  refreshFrame();
}

Void SubWindowHandle::seekAbsoluteEvent( UInt new_frame_num )
{
  if( m_pCurrStream )
  {
    m_pCurrStream->seekInput( new_frame_num );
    refreshFrame();
  }
}

Void SubWindowHandle::seekRelativeEvent( Bool bIsFoward )
{
  if( m_pCurrStream )
  {
    m_pCurrStream->seekInput( m_pCurrStream->getCurrFrameNum() + ( bIsFoward ? 1 : -1 ) );
    refreshFrame();
  }
}

Void SubWindowHandle::stop()
{
  m_bIsPlaying = false;
  seekAbsoluteEvent( 0 );
  return;
}

Void SubWindowHandle::normalSize()
{
  Double scaleFactor = 1.0;
  m_cViewArea->setZoomFactor( scaleFactor );
  updateLastScrollValue();
}

Void SubWindowHandle::zoomToFit()
{
  // Scale to a smaller size that the real to a nicer look
  QSize niceFit( m_cScrollArea->viewport()->size().width() - 5, m_cScrollArea->viewport()->size().height() - 5 );
  scaleView( niceFit );
}

Void SubWindowHandle::scaleView( Double scale )
{
  Q_ASSERT( m_cViewArea->image() );
  m_cViewArea->setZoomFactor( scale );
  updateLastScrollValue();
}

Void SubWindowHandle::scaleView( Int width, Int height )
{
  scaleView( QSize( width, height ) );
}

Void SubWindowHandle::scaleView( const QSize & size )
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

  wfactor = ( Double )newSize.width() / imgViewSize.width();
  hfactor = ( Double )newSize.height() / imgViewSize.height();

  if( wfactor < hfactor )
    scaleView( wfactor );
  else
    scaleView( hfactor );
}

Void SubWindowHandle::scaleViewByRatio( Double ratio )
{
  Q_ASSERT( m_cViewArea->image() );
  m_cViewArea->zoomChangeEvent( ratio, QPoint() );
}

void SubWindowHandle::adjustScrollBarByOffset( QPoint Offset )
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.x() ) );
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = m_cScrollArea->verticalScrollBar();
  scrollBar->setValue( int( scrollBar->value() + Offset.y() ) );
  m_cLastScroll.setY( scrollBar->value() );

  updateLastScrollValue();
}

// This function was developed with help of the schematics presented in
// http://stackoverflow.com/questions/13155382/jscrollpane-zoom-relative-to-mouse-position
void SubWindowHandle::adjustScrollBarByZoom( double factor, QPoint center )
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  if( center.isNull() )
  {
    scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
  }
  else
  {
    Int x = center.x() - m_cLastScroll.x();
    Int value = int( factor * m_cLastScroll.x() + ( ( factor - 1 ) * x ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    scrollBar->setValue( value );
  }

  scrollBar = m_cScrollArea->verticalScrollBar();
  if( center.isNull() )
  {
    scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
  }
  else
  {
    Int y = center.y() - m_cLastScroll.y();
    Int value = int( factor * m_cLastScroll.y() + ( ( factor - 1 ) * y ) );
    if( value > scrollBar->maximum() )
      value = scrollBar->maximum();
    if( value < scrollBar->minimum() )
      value = scrollBar->minimum();
    scrollBar->setValue( value );
  }

  updateLastScrollValue();
}

void SubWindowHandle::updateLastScrollValue()
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  m_cLastScroll.setX( scrollBar->value() );
  scrollBar = m_cScrollArea->verticalScrollBar();
  m_cLastScroll.setY( scrollBar->value() );
}

QSize SubWindowHandle::sizeHint() const
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
  else
    if( m_pCurrStream )
      isize = QSize( m_pCurrStream->getWidth() + 50, m_pCurrStream->getHeight() + 50 );

  // If the SubWindowHandle needs more space that the avaiable, we'll give
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

Bool SubWindowHandle::mayClose()
{
  return true;
}

void SubWindowHandle::closeEvent( QCloseEvent *event )
{
  Bool bAccept = mayClose();
  bAccept = m_bIsPlaying ? false : bAccept;
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
