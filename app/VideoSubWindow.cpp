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

#include <QStaticText>
#include "VideoSubWindow.h"
#include "ModulesHandle.h"
#include "ConfigureFormatDialog.h"
#include "SubWindowAbstract.h"
#include <QScrollArea>
#if( QT_VERSION_PLAYUVER == 5 )
#include "QtConcurrent/qtconcurrentrun.h"
#endif

namespace plaYUVer
{

/**
 * \brief Functions to control data stream from stream information
 */

QDataStream& operator<<( QDataStream& out, const PlaYUVerStreamInfoVector& array )
{
  PlaYUVerStreamInfo d;
  out << array.size();
  for( Int i = 0; i < array.size(); i++ )
  {
    d = array.at( i );
    out << d.m_cFilename
        << d.m_uiWidth
        << d.m_uiHeight
        << d.m_iPelFormat
        << d.m_uiBitsPelPixel
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
    in >> d.m_uiBitsPelPixel;
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

/**
 * \class VideoInformation
 * \brief Shows information about video sub window
 */

class VideoInformation: public QWidget
{
private:
  QList<QStaticText> m_cTopLeftTextList;
  QFont m_cTopLeftTextFont;
  QFont m_cCenterTextFont;
  Bool m_bBusyWindow;
public:
  VideoInformation( QWidget *parent ) :
          QWidget( parent ),
          m_bBusyWindow( false )
  {
    setPalette( Qt::transparent );
    setAttribute( Qt::WA_TransparentForMouseEvents );
    m_cTopLeftTextFont.setPointSize( 8 );
    m_cCenterTextFont.setPointSize( 12 );
  }
  Void setInformationTopLeft( const QStringList& textLines )
  {
    m_cTopLeftTextList.clear();
    for( Int i = 0; i < textLines.size(); i++ )
    {
      m_cTopLeftTextList.append( textLines.at( i ) );
    }
  }
  Void setBusyWindow( Bool bFlag )
  {
    m_bBusyWindow = bFlag;
  }
protected:
  void paintEvent( QPaintEvent *event )
  {
    QPainter painter( this );
    painter.setFont( m_cTopLeftTextFont );
    if( !m_cTopLeftTextList.isEmpty() && size().width() > 300 )
    {
      QPoint topLeftCorner( 10, 10 );
      for( Int i = 0; i < m_cTopLeftTextList.size(); i++ )
      {
        painter.drawStaticText( topLeftCorner, m_cTopLeftTextList.at( i ) );
        topLeftCorner += QPoint( 0, 15 );
      }
    }
    if( m_bBusyWindow )
    {
      painter.setFont( m_cCenterTextFont );
      painter.drawText( rect(), Qt::AlignHCenter | Qt::AlignVCenter, QStringLiteral( "Refreshing..." ) );
      painter.fillRect( rect(), QBrush( QColor::fromRgb( 255, 255, 255, 50 ), Qt::SolidPattern ) );
    }
  }
};

VideoSubWindow::VideoSubWindow( enum VideoSubWindowCategories category, QWidget * parent ) :
        SubWindowAbstract( parent, SubWindowAbstract::VIDEO_SUBWINDOW | category ),
        m_bWindowBusy( false ),
        m_pCurrStream( NULL ),
        m_pcCurrFrame( NULL ),
        m_pcCurrentDisplayModule( NULL ),
        m_pcReferenceSubWindow( NULL ),
        m_bIsPlaying( false )
{

  // Create a new scroll area inside the sub-window
  m_pcScrollArea = new QScrollArea;
  connect( m_pcScrollArea->horizontalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateCurScrollValues() ) );
  connect( m_pcScrollArea->verticalScrollBar(), SIGNAL( actionTriggered( int ) ), this, SLOT( updateCurScrollValues() ) );

  // Create a new interface to show images
  m_cViewArea = new ViewArea;
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SLOT( adjustScrollBarByScale( double, QPoint ) ) );
  connect( m_cViewArea, SIGNAL( zoomFactorChanged_byWheel( double , QPoint) ), this, SIGNAL( zoomFactorChanged_SWindow( double, QPoint ) ) );

  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SLOT( adjustScrollBarByOffset( QPoint ) ) );
  connect( m_cViewArea, SIGNAL( scrollBarMoved( QPoint ) ), this, SIGNAL( scrollBarMoved_SWindow( QPoint ) ) );

  connect( m_cViewArea, SIGNAL( selectionChanged( QRect ) ), this, SLOT( updateSelectedArea( QRect ) ) );
  connect( m_cViewArea, SIGNAL( positionChanged( const QPoint & ) ), this, SLOT( updatePixelValueStatusBar( const QPoint & ) ) );

  m_pcScrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  m_pcScrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  // Define the cViewArea as the widget inside the scroll area
  m_pcScrollArea->setWidget( m_cViewArea );
  m_pcScrollArea->setWidgetResizable( true );

  setWidget( m_pcScrollArea );

  m_cLastScroll = QPoint();

  m_apcCurrentModule.clear();

  m_pcUpdateTimer = new QTimer();
  m_pcUpdateTimer->setInterval( 800 );
  connect( m_pcUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateWindowOnTimeout() ) );

  //! Add video information
  m_pcVideoInfo = new VideoInformation( this );
}

VideoSubWindow::~VideoSubWindow()
{
  m_pcCurrFrame = NULL;
  disableModule();
  delete m_cViewArea;
  if( m_pCurrStream )
    delete m_pCurrStream;
}

/**
 * Events handlers
 */

Void VideoSubWindow::keyPressEvent( QKeyEvent* event )
{
  if( m_pcCurrentDisplayModule )
  {
    if( m_pcCurrentDisplayModule->getModuleRequirements() & MODULE_USES_KEYS )
    {
      Bool bRet = false;
      switch( event->key() )
      {
      case Qt::Key_A:
        bRet = m_pcCurrentDisplayModule->getModule()->keyPressed( MODULE_KEY_LEFT );
        break;
      case Qt::Key_D:
        bRet = m_pcCurrentDisplayModule->getModule()->keyPressed( MODULE_KEY_RIGHT );
        break;
      case Qt::Key_W:
        bRet = m_pcCurrentDisplayModule->getModule()->keyPressed( MODULE_KEY_UP );
        break;
      case Qt::Key_S:
        bRet = m_pcCurrentDisplayModule->getModule()->keyPressed( MODULE_KEY_DOWN );
        break;
      }
      if( bRet )
      {
        refreshFrame();
        return;
      }
    }
  }
  QWidget::keyPressEvent( event );
}

Void VideoSubWindow::resizeEvent( QResizeEvent* event )
{
  m_pcVideoInfo->resize( event->size() );
}

/**
 * Size related handlers
 */

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

Void VideoSubWindow::updateWindowOnTimeout()
{
  m_pcVideoInfo->update();
}

Void VideoSubWindow::loadAll()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  m_pCurrStream->loadAll();
  refreshFrame();
  QApplication::restoreOverrideCursor();
}

Void VideoSubWindow::refreshSubWindow()
{
  if( getCategory() & SubWindowAbstract::VIDEO_STREAM_SUBWINDOW )
  {
    if( !m_pCurrStream->reload() )
    {
      close();
      return;
    }
    //seekAbsoluteEvent( currFrameNum );
  }
  updateVideoWindowInfo();
  refreshFrame();
}

Bool VideoSubWindow::loadFile( QString cFilename, Bool bForceDialog )
{
  ConfigureFormatDialog formatDialog( this );
  UInt Width = 0, Height = 0, BitsPel = 8, FrameRate = 30;
  Int InputFormat = PlaYUVerFrame::YUV420p;

  if( m_pCurrStream )
    m_pCurrStream->getFormat( Width, Height, InputFormat, BitsPel, FrameRate );

  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }

  if( guessFormat( cFilename, Width, Height, InputFormat, FrameRate ) || bForceDialog )
  {
    if( formatDialog.runConfigureFormatDialog( QFileInfo( cFilename ).fileName(), Width, Height, InputFormat, BitsPel, FrameRate ) == QDialog::Rejected )
    {
      return false;
    }
  }

  try
  {
    m_pCurrStream->open( cFilename.toStdString(), Width, Height, InputFormat, BitsPel, FrameRate );
  }
  catch( const char *msg )
  {
    if( formatDialog.runConfigureFormatDialog( QFileInfo( cFilename ).fileName(), Width, Height, InputFormat, BitsPel, FrameRate ) == QDialog::Rejected )
    {
      return false;
    }
    m_pCurrStream->open( cFilename.toStdString(), Width, Height, InputFormat, BitsPel, FrameRate );
  }

  m_sStreamInfo.m_cFilename = cFilename;
  m_sStreamInfo.m_uiWidth = Width;
  m_sStreamInfo.m_uiHeight = Height;
  m_sStreamInfo.m_iPelFormat = InputFormat;
  m_sStreamInfo.m_uiBitsPelPixel = BitsPel;
  m_sStreamInfo.m_uiFrameRate = FrameRate;
  m_sStreamInfo.m_uiFileSize = QFileInfo( cFilename ).size();

  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = cFilename;

  updateVideoWindowInfo();

  return true;
}

Bool VideoSubWindow::loadFile( PlaYUVerStreamInfo* streamInfo )
{
  if( !m_pCurrStream )
  {
    m_pCurrStream = new PlaYUVerStream;
  }

  if( !m_pCurrStream->open( streamInfo->m_cFilename.toStdString(), streamInfo->m_uiWidth, streamInfo->m_uiHeight, streamInfo->m_iPelFormat,
      streamInfo->m_uiBitsPelPixel, streamInfo->m_uiFrameRate ) )
  {
    return false;
  }

  m_sStreamInfo = *streamInfo;
  m_cViewArea->setInputStream( m_pCurrStream );

  QApplication::restoreOverrideCursor();

  refreshFrame();

  m_cFilename = streamInfo->m_cFilename;
  updateVideoWindowInfo();
  setWindowName( QFileInfo( m_cFilename ).fileName() );
  return true;
}

Void VideoSubWindow::updateVideoWindowInfo()
{
  m_cStreamInformation = "";
  if( m_pcCurrentDisplayModule )
  {
    m_cStreamInformation = "Module";
    QList<VideoSubWindow*> arraySubWindows = m_pcCurrentDisplayModule->getSubWindowList();
    QStringList sourceWindowList;
    if( arraySubWindows.size() > 0 )
    {
      for( Int i = 0; i < arraySubWindows.size(); i++ )
      {
        if( arraySubWindows.at( i )->getWindowName() != getWindowName() )
        {
          sourceWindowList.append( QString( "Input %1 - " + arraySubWindows.at( i )->getWindowName() ).arg( i + 1 ) );
        }
      }
    }
    //qDebug( ) << sourceWindowList;
    if( sourceWindowList.size() > 0 )
    {
      m_pcVideoInfo->setInformationTopLeft( sourceWindowList );
    }
  }
  else if( m_pCurrStream )
  {
    QString m_cFormatName = QString::fromStdString( m_pCurrStream->getFormatName() );
    QString m_cCodedName = QString::fromStdString( m_pCurrStream->getCodecName() );
    m_cStreamInformation = m_cFormatName + " | " + m_cCodedName;
  }
//  if( m_pcCurrFrame )
//  {
//    QString m_cPelFmtName = QString::fromStdString( m_pcCurrFrame->getPelFmtName() );
//    m_cStreamInformation += " | " + m_cPelFmtName;
//  }
  if( m_cStreamInformation.isEmpty() )
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
          frame_bytes = PlaYUVerFrame::getBytesPerFrame( stdResList[i].uiWidth, stdResList[i].uiHeight, rInputFormat, 8 );
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
    refreshFrame();
    updateVideoWindowInfo();
  }
}

Void VideoSubWindow::associateModule( PlaYUVerAppModuleIf* pcModule )
{
  m_apcCurrentModule.append( pcModule );
}

Void VideoSubWindow::setFillWindow( Bool bFlag )
{
  m_pcVideoInfo->setBusyWindow( bFlag );
}
Void VideoSubWindow::setCurrFrame( PlaYUVerFrame* pcCurrFrame )
{
// if( m_pcCurrFrame )
  {
    m_pcCurrFrame = pcCurrFrame;
    m_cViewArea->setImage( m_pcCurrFrame );
  }
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
    m_bWindowBusy = true;
    ModulesHandle::applyModuleIf( m_pcCurrentDisplayModule, m_bIsPlaying );
    bSetFrame = false;
  }
  if( bSetFrame )
  {
    m_cViewArea->setImage( m_pcCurrFrame );
  }
  for( Int i = 0; i < m_apcCurrentModule.size(); i++ )
  {
    m_bWindowBusy = true;
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

QSize VideoSubWindow::getScrollSize()
{
  return QSize( m_pcScrollArea->viewport()->size().width() - 5, m_pcScrollArea->viewport()->size().height() - 5 );
}

Void VideoSubWindow::adjustScrollBarByOffset( QPoint Offset )
{
  QPoint cLastScroll = m_cCurrScroll;
  QScrollBar *scrollBarH, *scrollBarV;
  Int valueX, valueY;

  valueX = int( cLastScroll.x() + Offset.x() );
  valueY = int( cLastScroll.y() + Offset.y() );

  scrollBarH = m_pcScrollArea->horizontalScrollBar();
  if( valueX > scrollBarH->maximum() )
    valueX = scrollBarH->maximum();
  if( valueX < scrollBarH->minimum() )
    valueX = scrollBarH->minimum();
  m_cCurrScroll.setX( valueX );

  scrollBarV = m_pcScrollArea->verticalScrollBar();
  if( valueY > scrollBarV->maximum() )
    valueY = scrollBarV->maximum();
  if( valueY < scrollBarV->minimum() )
    valueY = scrollBarV->minimum();
  m_cCurrScroll.setY( valueY );

// Update window scroll
  scrollBarH->setValue( m_cCurrScroll.x() );
  scrollBarV->setValue( m_cCurrScroll.y() );
}

// This function was developed with help of the schematics presented in
// http://stackoverflow.com/questions/13155382/jscrollpane-zoom-relative-to-mouse-position
Void VideoSubWindow::adjustScrollBarByScale( Double scale, QPoint center )
{
  QPoint cLastScroll = m_cCurrScroll;
  QScrollBar *scrollBarH, *scrollBarV;

  scrollBarH = m_pcScrollArea->horizontalScrollBar();
  if( center.isNull() )
  {
    m_cCurrScroll.setX( int( scale * cLastScroll.x() + ( ( scale - 1 ) * scrollBarH->pageStep() / 2 ) ) );
  }
  else
  {
    Int x = center.x() - cLastScroll.x();
    Int value = int( scale * cLastScroll.x() + ( ( scale - 1 ) * x ) );
    if( value > scrollBarH->maximum() )
      value = scrollBarH->maximum();
    if( value < scrollBarH->minimum() )
      value = scrollBarH->minimum();
    m_cCurrScroll.setX( value );
  }

  scrollBarV = m_pcScrollArea->verticalScrollBar();
  if( center.isNull() )
  {
    m_cCurrScroll.setY( int( scale * cLastScroll.y() + ( ( scale - 1 ) * scrollBarV->pageStep() / 2 ) ) );
  }
  else
  {
    Int y = center.y() - cLastScroll.y();
    Int value = int( scale * cLastScroll.y() + ( ( scale - 1 ) * y ) );
    if( value > scrollBarV->maximum() )
      value = scrollBarV->maximum();
    if( value < scrollBarV->minimum() )
      value = scrollBarV->minimum();
    m_cCurrScroll.setY( value );
  }

// Update window scroll
  scrollBarH->setValue( m_cCurrScroll.x() );
  scrollBarV->setValue( m_cCurrScroll.y() );

}

Void VideoSubWindow::updateCurScrollValues()
{
  QScrollBar *scrollBar = m_pcScrollArea->horizontalScrollBar();
  m_cCurrScroll.setX( scrollBar->value() );
  scrollBar = m_pcScrollArea->verticalScrollBar();
  m_cCurrScroll.setY( scrollBar->value() );
}

Void VideoSubWindow::setCurScrollValues()
{
  QScrollBar *scrollBar = m_pcScrollArea->horizontalScrollBar();
  scrollBar->setValue( m_cCurrScroll.x() );
  scrollBar = m_pcScrollArea->verticalScrollBar();
  scrollBar->setValue( m_cCurrScroll.y() );
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

//Void VideoSubWindow::closeEvent( QCloseEvent *event )
//{
//  Bool bAccept = m_bIsPlaying ? false : true;
//  if( bAccept )
//  {
//    event->accept();
//  }
//  else
//  {
//    event->ignore();
//  }
//}

}// NAMESPACE
