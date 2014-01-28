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
 * \file     imageinterface.h
 * \brief    Sub windows handling
 */

#ifndef __IMAGEINTERFACE_H__
#define __IMAGEINTERFACE_H__

#include <QMdiSubWindow>
#include <QScrollArea>
#include <QLabel>
#include <QMdiArea>
#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QColor>

#include "TypeDef.h"
#include "InputStream.h"
#include "viewarea.h"

namespace plaYUVer
{

using namespace SCode;

class ImageInterface: public QScrollArea
{
Q_OBJECT

private:

  //QLabel* m_cViewArea;
  ViewArea* m_cViewArea;

  QString m_cCurrFileName;

  InputStream m_currStream;

  Bool isUntitled;

  Double m_dScaleFactor;
  Bool fileExists;
  Bool m_firsTime;

public:
  ImageInterface( QWidget * parent = 0 );
  ~ImageInterface();

  bool loadFile( const QString &fileName );
  bool save();
  bool saveAs();
  bool saveFile( const QString &fileName );

  /**
   * Show the image at its original size
   */
  Void normalSize();
  /**
   * Scale the image (zoomed in or out) to fit on the window.
   */
  Void zoomToFit();
  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  Void scaleView( Double factor );

  /**
   * The current image size is scaled to a rectangle as large as possible
   * inside (@p width, @p height ) preserving the aspect ratio.
   */
  Void scaleView( Int width, Int height );

  /**
   * This is an overloaded member function, provided for convenience.
   * Scales the image to a rectangle with the given size, preserving the
   * aspect ratio.
   */
  Void scaleView( const QSize & size );

  Void adjustScrollBar( Double factor );

  Double getScaleFactor()
  {
    return m_dScaleFactor;
  }

  QSize sizeHint() const;

  /**
   * Arranges all the listed widgets (normaly QMdiImageInterface) in a cascade
   * pattern.
   * @Note This function implementation is taken from the Qt source. The
   * QMdiArea only have a function that arranges @b all the child windows.
   */
  static void cascader( QList<QWidget *> &widgets, const QRect &domain );

  QString userFriendlyCurrentFile();
  QString strippedName( const QString &fullFileName );
  QString currentFile()
  {
    return m_cCurrFileName;
  }

protected:
  Void closeEvent( QCloseEvent *event );

private slots:

};

} // NAMESPACE

#endif // __IMAGEINTERFACE_H__
