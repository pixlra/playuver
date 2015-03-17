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
#include "ModulesHandle.h"
#include "SubWindowHandle.h"
#include "ConfigureFormatDialog.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include "QtConcurrent/qtconcurrentrun.h"
#endif

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
                         << d.m_uiFrameRate
                         << d.m_uiFileSize;
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
    in >> d.m_uiFileSize;
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

VideoSubWindow::VideoSubWindow( enum VideoSubWindowCategories category, QWidget * parent ) :
        SubWindowHandle( parent, SubWindowHandle::VIDEO_SUBWINDOW | category ),
        m_pCurrStream( NULL ),
        m_pcCurrFrame( NULL ),
        m_pcCurrentDisplayModule( NULL ),
        m_pcReferenceSubWindow( NULL ),
        m_bIsPlaying( false ),
        m_bIsModule( category == MODULE_SUBWINDOW )
{

  // Create a new interface to show images
  m_cViewArea = new ViewArea( this );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SLOT( adjustScrollBarByScale( double, QPoint ) ) );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SIGNAL( zoomFactorChanged_SWindow( double, QPoint ) ) );

  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SLOT( adjustScrollBarByOffset( QPoint ) ) );
  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SIGNAL( scrollBarMoved_SWindow( QPoint ) ) );

  connect( m_cViewArea, SIGNAL( selectionChanged( QRect ) ), this, SLOT( updateSelectedArea( QRect ) ) );
  connect( m_cViewArea, SIGNAL( positionChanged( const QPoint & ) ), this, SLOT( updatePixelValueStatusBar( const QPoint & ) ) );

  setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  // Define the cViewArea as the widget inside the scroll area
  m_cViewArea->setMinimumSize( size() );
  setWidget( m_cViewArea );

  m_apcCurrentModule.clear();

}

VideoSubWindow::~VideoSubWindow()
{
  disableModule();
  delete m_cViewArea;
  if( m_pCurrStream )
    delete m_pCurrStream;
}

Void VideoSubWindow::loadAll()
{
  m_pCurrStream->loadAll();
  refreshFrame();
}

Void VideoSubWindow::refreshSubWindow()
{
  if( getCategory() == SubWindowHandle::VIDEO_STREAM_SUBWINDOW )
  {
    Int currFrameNum = m_pCurrStream->getCurrFrameNum();
    if( !loadFile( m_cFilename, false ) )
    {
      close();
      return;
    }
    seekAbsoluteEvent( currFrameNum );
  }
  else
  {
    refreshFrame();
  }
}

Bool VideoSubWindow::loadFile( QString cFilename, Bool bForceDialog )
{
  UInt Width = 0, Height = 0, FrameRate = 30;
  Int InputFormat = PlaYUVerFrame::YUV420p;

  if( m_pCurrStream )
    m_pCurrStream->getFormat( Width, Height, InputFormat, FrameRate );

  if( guessFormat( cFilename, Width, Height, InputFormat, FrameRate ) || bForceDialog )
  {
    ConfigureFormatDialog formatDialog( this );
    if( formatDialog.runConfigureFormatDialog( QFileInfo( cFilename ).fileName(), Width, Height, InputFormat, FrameRate ) == QDialog::Rejected )
    {
      return false;
    }
  }

  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }
  else
  {
    delete m_pCurrStream;
    m_pCurrStream = new PlaYUVerStream;
  }

  m_pCurrStream->open( cFilename.toStdString(), Width, Height, InputFormat, FrameRate );

  m_sStreamInfo.m_cFilename = cFilename;
  m_sStreamInfo.m_uiWidth = Width;
  m_sStreamInfo.m_uiHeight = Height;
  m_sStreamInfo.m_iPelFormat = InputFormat;
  m_sStreamInfo.m_uiFrameRate = FrameRate;
  m_sStreamInfo.m_uiFileSize = QFileInfo( cFilename ).size();

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = cFilename;
  m_cWindowShortName = QFileInfo( cFilename ).fileName();

  updateVideoWindowInfo();

  setWindowName( m_cWindowShortName );
  return true;
}

Bool VideoSubWindow::loadFile( PlaYUVerStreamInfo* streamInfo )
{
  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }

  if( !m_pCurrStream->open( streamInfo->m_cFilename.toStdString(), streamInfo->m_uiWidth, streamInfo->m_uiHeight, streamInfo->m_iPelFormat,
      streamInfo->m_uiFrameRate ) )
  {
    return false;
  }

  m_sStreamInfo = *streamInfo;

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = streamInfo->m_cFilename;
  m_cWindowShortName = QFileInfo( streamInfo->m_cFilename ).fileName();

  updateVideoWindowInfo();

  setWindowName( m_cWindowShortName );
  return true;
}

Void VideoSubWindow::updateVideoWindowInfo()
{
  if( m_pCurrStream )
  {
    QString m_cFormatName = QString::fromStdString( m_pCurrStream->getFormatName() );
    QString m_cCodedName = QString::fromStdString( m_pCurrStream->getCodecName() );
    QString m_cPelFmtName = QString::fromStdString( m_pCurrStream->getPelFmtName() );
    m_cStreamInformation = m_cFormatName + " | " + m_cCodedName + " | " + m_cPelFmtName;
  }
  else if( m_pcCurrFrame )
  {
    if( m_bIsModule || m_pcCurrentDisplayModule )
    {
      m_cStreamInformation = "Module | ";
    }
    QString m_cPelFmtName = QString::fromStdString( PlaYUVerFrame::supportedPixelFormatListNames()[m_pcCurrFrame->getPelFormat()].c_str() );
    m_cStreamInformation += m_cPelFmtName;
  }
  else
  {
    m_cStreamInformation = "          ";
  }
}

Bool VideoSubWindow::guessFormat( QString filename, UInt& rWidth, UInt& rHeight, Int& rInputFormat, UInt& rFrameRate )
{
  std::vector<PlaYUVerStdResolution> stdResList = PlaYUVerStream::stdResolutionSizes();
  Bool bGuessed = true;
  Bool bGuessedByFilesize = false;
  QString FilenameShort = QFileInfo( filename ).fileName();
  QString fileExtension = QFileInfo( filename ).suffix();
  if( !fileExtension.compare( "yuv" ) )
  {
    bGuessed = false;
    // Guess pixel format
    QVector<std::string> formats_list = QVector<std::string>::fromStdVector( PlaYUVerFrame::supportedPixelFormatListNames() );
    for( Int i = 0; i < formats_list.size(); i++ )
    {
      if( FilenameShort.contains( formats_list.at( i ).c_str(), Qt::CaseInsensitive ) )
      {
        rInputFormat = i;
        break;
      }
    }

    if( rWidth == 0 || rHeight == 0 )
    {
      // Guess resolution - match  resolution name
      Int iMatch = -1;
      for( UInt i = 0; i < stdResList.size(); i++ )
      {
        if( FilenameShort.contains( QString::fromStdString( stdResList[i].shortName ) ) )
        {
          iMatch = i;
        }
      }
      if( iMatch >= 0 )
      {
        rWidth = stdResList[iMatch].uiWidth;
        rHeight = stdResList[iMatch].uiHeight;
      }
      // Guess resolution - match %dx%d
#if( QT_VERSION_PLAYUVER == 5 )
      QRegularExpressionMatch resolutionMatch = QRegularExpression( "_\\d*x\\d*" ).match( FilenameShort );
      if( resolutionMatch.hasMatch() )
      {
        QString resolutionString = resolutionMatch.captured( 0 );
        if( resolutionString.startsWith( "_" ) || resolutionString.endsWith( "_" ) )
        {
          resolutionString.remove( "_" );
          QStringList resolutionArgs = resolutionString.split( "x" );
          if( resolutionArgs.size() == 2 )
          {
            rWidth = resolutionArgs.at( 0 ).toUInt();
            rHeight = resolutionArgs.at( 1 ).toUInt();
          }
        }
      }
#endif
    }

    // Guess resolution by file size
    if( rWidth == 0 && rHeight == 0 )
    {
      Char *filename_char = new char[filename.length() + 1];
      memcpy( filename_char, filename.toUtf8().constData(), filename.length() + 1 * sizeof(char) );
      FILE* pF = fopen( filename_char, "rb" );
      if( pF )
      {
        fseek( pF, 0, SEEK_END );
        UInt64 uiFileSize = ftell( pF );
        fclose( pF );
        delete[] filename_char;

        Int count = 0, module, frame_bytes, match;
        for( UInt i = 0; i < stdResList.size(); i++ )
        {
          frame_bytes = PlaYUVerFrame::getBytesPerFrame( stdResList[i].uiWidth, stdResList[i].uiHeight, rInputFormat );
          module = uiFileSize % frame_bytes;
          if( module == 0 )
          {
            match = i;
            count++;
          }
        }
        if( count == 1 )
        {
          rWidth = stdResList[match].uiWidth;
          rHeight = stdResList[match].uiHeight;
          bGuessedByFilesize = true;
        }
      }
    }

    if( rWidth > 0 && rHeight > 0 && rInputFormat >= 0 )
      bGuessed = true && !bGuessedByFilesize;
  }
  return !bGuessed;
}

Void VideoSubWindow::updateSelectedArea( QRect area )
{
  m_cSelectedArea = area;
}

/**
 * Functions to enable a module in the
 * current SubWindow
 */
Void VideoSubWindow::enableModule( PlaYUVerAppModuleIf* pcModule )
{
  if( m_pcCurrentDisplayModule )
  {
    disableModule( m_pcCurrentDisplayModule );
  }
  m_pcCurrentDisplayModule = pcModule;
  updateVideoWindowInfo();
}

Void VideoSubWindow::disableModule( PlaYUVerAppModuleIf* pcModule )
{
  Bool bRefresh = false;
  if( pcModule )
  {
    Int modIdx = m_apcCurrentModule.indexOf( pcModule );
    if( modIdx != -1 )
    {
      m_apcCurrentModule.removeAt( modIdx );
      ModulesHandle::destroyModuleIf( pcModule );
      bRefresh |= true;
    }
    if( pcModule == m_pcCurrentDisplayModule )
    {
      pcModule = m_pcCurrentDisplayModule;
      m_pcCurrentDisplayModule = 0;
      ModulesHandle::destroyModuleIf( pcModule );
      bRefresh |= true;
    }
  }
  else
  {
    QList<PlaYUVerAppModuleIf*> apcCurrentModule = m_apcCurrentModule;
    for( Int i = 0; i < apcCurrentModule.size(); i++ )
    {
      m_apcCurrentModule.removeOne( apcCurrentModule.at( i ) );
      ModulesHandle::destroyModuleIf( apcCurrentModule.at( i ) );
      bRefresh |= true;
    }
    assert( m_apcCurrentModule.size() == 0 );
    if( m_pcCurrentDisplayModule )
    {
      pcModule = m_pcCurrentDisplayModule;
      m_pcCurrentDisplayModule = 0;
      ModulesHandle::destroyModuleIf( pcModule );
      bRefresh |= true;
    }
  }
  if( bRefresh )
  {
    updateVideoWindowInfo();
    refreshFrame();
  }
}

Void VideoSubWindow::associateModule( PlaYUVerAppModuleIf* pcModule )
{
  m_apcCurrentModule.append( pcModule );
}

Void VideoSubWindow::setCurrFrame( PlaYUVerFrame* pcCurrFrame )
{
  m_pcCurrFrame = pcCurrFrame;
  m_cViewArea->setImage( m_pcCurrFrame );
}

Void VideoSubWindow::refreshFrameOperation()
{
  Bool bSetFrame = false;
  if( m_pCurrStream )
  {
    m_pcCurrFrame = m_pCurrStream->getCurrFrame();
    bSetFrame = m_pcCurrFrame ? true : false;
  }
  if( m_pcCurrentDisplayModule )
  {
    ModulesHandle::applyModuleIf( m_pcCurrentDisplayModule, m_bIsPlaying );
    bSetFrame = false;
  }
  if( bSetFrame )
  {
    m_cViewArea->setImage( m_pcCurrFrame );
  }
  for( Int i = 0; i < m_apcCurrentModule.size(); i++ )
  {
    ModulesHandle::applyModuleIf( m_apcCurrentModule.at( i ), m_bIsPlaying );
  }
}

Void VideoSubWindow::refreshFrame( Bool bThreaded )
{
#ifndef QT_NO_CONCURRENT
  m_cRefreshResult.waitForFinished();
  if( bThreaded )
  {
    m_cRefreshResult = QtConcurrent::run( this, &VideoSubWindow::refreshFrameOperation );
  }
  else
#endif
  {
    refreshFrameOperation();
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
  iRet = PlaYUVerStream::saveFrame( filename.toStdString(), saveFrame );
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
  Bool bEndOfSeq = false;
  if( m_pCurrStream && m_bIsPlaying )
  {
#ifndef QT_NO_CONCURRENT
    m_cRefreshResult.waitForFinished();
    m_cReadResult.waitForFinished();
#endif
    bEndOfSeq = m_pCurrStream->setNextFrame();
    if( !bEndOfSeq )
    {
      refreshFrame();
#ifndef QT_NO_CONCURRENT
      m_cReadResult = QtConcurrent::run( m_pCurrStream, &PlaYUVerStream::readFrameFillRGBBuffer );
#else
      m_pCurrStream->readFrameFillRGBBuffer();
#endif
    }
  }
  return bEndOfSeq;
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
    if( m_pCurrStream->seekInputRelative( bIsFoward ) )
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
  Double curFactor = getScaleFactor();
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor / curFactor, QPoint(), QSize() );
    adjustScrollBarByScale( usedScale, QPoint() );
  }
}

Void VideoSubWindow::zoomToFit()
{
// Scale to a smaller size that the real to a nicer look
  scaleView( getScrollSize() );
}

Void VideoSubWindow::zoomToFactor( Double factor, QPoint center )
{
  Double curFactor;
  curFactor = getScaleFactor();
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor / curFactor, center, QSize() );
    adjustScrollBarByScale( usedScale, center );
  }
}

Void VideoSubWindow::scaleView( Double scale, QPoint center )
{
  Q_ASSERT( m_cViewArea->image() );
  if( scale != 1.0 )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( scale, center, getScrollSize() );
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

  Double curFactor = getScaleFactor();
  if( factor != curFactor )
  {
    Double usedScale;
    usedScale = m_cViewArea->scaleZoomFactor( factor / curFactor, center, getScrollSize() );
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
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y() );
        strStatus.append( QString( "Y: %1" ).arg( sPixelValue.Y() ) );
      }
      if( ColorSpace == PlaYUVerFrame::COLOR_YUV )
      {
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y() );
        strStatus.append( QString( "Y: %1   U: %2   V: %3" ).arg( sPixelValue.Y() ).arg( sPixelValue.Cb() ).arg( sPixelValue.Cr() ) );
      }

      if( ( ColorSpace == PlaYUVerFrame::COLOR_RGB ) )
      {
        sPixelValue = m_pcCurrFrame->getPixelValue( pos.x(), pos.y() );
        strStatus.append( QString( "R: %1   G: %2   B: %3" ).arg( sPixelValue.R() ).arg( sPixelValue.G() ).arg( sPixelValue.B() ) );
      }
      emit updateStatusBar( strStatus );
    }
  }
}

QSize VideoSubWindow::sizeHint() const
{
  QSize maxSize;

  QWidget *p = parentWidget();
  if( p )
  {
    maxSize = p->size();
  }
  else
  {
    maxSize = QApplication::desktop()->availableGeometry().size();
  }
  return sizeHint( maxSize );
}

QSize VideoSubWindow::sizeHint( const QSize & maxSize ) const
{
  QSize isize;
  if( m_pcCurrFrame )
    isize = QSize( m_pcCurrFrame->getWidth() + 50, m_pcCurrFrame->getHeight() + 50 );
  else if( m_pCurrStream )
    isize = QSize( m_pCurrStream->getWidth() + 50, m_pCurrStream->getHeight() + 50 );

// If the VideoSubWindow needs more space that the avaiable, we'll give
// to the subwindow a reasonable size preserving the image aspect ratio.
  if( !( isize.width() < maxSize.width() && isize.height() < maxSize.height() ) )
  {
    isize.scale( maxSize, Qt::KeepAspectRatio );
  }
  return isize;
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
