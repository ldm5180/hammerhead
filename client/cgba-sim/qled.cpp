/***************************************************************************
 *   Copyright (C) 2010 by P. Sereno                                       *
 *   http://www.sereno-online.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *   http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.               *
 ***************************************************************************/
  
#include <QColor>
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>
#include <QtSvg>
#include <QSvgRenderer>
#include "qled.h"


/*!
  \brief QLed: this is the QLed constructor.
  \param parent: The Parent Widget
*/
QLed::QLed(QWidget *parent)
    : QWidget(parent)
{
   m_value=false;
   m_onColor=Red;
   m_offColor=Grey;
   m_shape=Square;
   setMinimumSize(QSize(50,50));
   setMaximumSize(QSize(125,125));
   shapes << "resources/square_";
   colors << "red.svg" << "grey.svg";
}

/*!
  \brief paintEvent: painting method
  \param QPaintEvent *
  \return void
*/
void QLed::paintEvent(QPaintEvent *)
{
    QSvgRenderer *renderer = new QSvgRenderer();
    QString ledShapeAndColor;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    ledShapeAndColor=shapes[m_shape];

    if(m_value)
        ledShapeAndColor.append(colors[m_onColor]);
    else
        ledShapeAndColor.append(colors[m_offColor]);

    renderer->load(ledShapeAndColor);
    renderer->render(&painter);

}

/*!
  \brief setValue: this method allows to set the led value {true,false}
  \param ledColor newColor
  \return void
*/
void QLed::setValue(bool value)
{
   m_value=value;
   update();
}


/*!
  \brief toggleValue: this method toggles the led value
  \param ledColor newColor
  \return void
*/
void QLed::toggleValue()
{ 
	m_value=!m_value;
	update();
	return; 
}

QLed::~QLed()
{
    delete label;
}