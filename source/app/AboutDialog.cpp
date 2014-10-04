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
 * \file     AboutDialog.cpp
 * \brief    About Dialog
 *           Based on the work of Glad Deschrijver <glad.deschrijver@gmail.com> in KTikZ project
 */


#include "AboutDialog.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
  QPixmap logo = QPixmap(":/images/playuver-backgroud-logo.png").scaled( QSize( 350,350), Qt::KeepAspectRatio );
	QLabel *pixmapLabel = new QLabel;
	pixmapLabel->setPixmap( logo );


	QLabel *label = new QLabel(QString("<h1>%1 Version %2</h1><p>%3</p><p>%4</p>")
	    .arg(QApplication::applicationName())
	    .arg(QApplication::applicationVersion())
	    .arg(tr("Enhanced open-source QT based raw video player"))
	    .arg(tr("Copyright 2014 Luis Lucas and Joao Carreira")));
	label->setWordWrap(true);

	QWidget *topWidget = new QWidget;
	QVBoxLayout *topLayout = new QVBoxLayout;
	topLayout->addWidget(pixmapLabel);
	topLayout->addWidget(label);
	topWidget->setLayout(topLayout);

//	QTextEdit *textEdit = new QTextEdit(tr("<p>This program is free "
//	    "software; you can redistribute it and/or modify it under the "
//	    "terms of the GNU General Public License as published by the "
//	    "Free Software Foundation; either version 2 of the License, "
//	    "or (at your option) any later version.</p>"
//	    "<p>This program is distributed in the hope that it will "
//	    "be useful, but WITHOUT ANY WARRANTY; without even the implied "
//	    "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  "
//	    "See the GNU General Public License for more details.</p>"));
//	textEdit->setReadOnly(true);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(topWidget);
	//mainLayout->addWidget(textEdit);
	mainLayout->addWidget(buttonBox);
	mainLayout->setSpacing(10);
	buttonBox->setFocus();

	setWindowTitle(tr("About %1").arg(QApplication::applicationName()));
}
