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

#ifndef QLED_H
#define QLED_H

#include <QtGui>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QColor>
#include <QtGlobal>
#include <QPolygon>
#include <QtSvg/QSvgRenderer>

class QColor;

class QLed : public QWidget
{
 Q_OBJECT
        Q_ENUMS (ledColor)
        Q_ENUMS (ledShape)
	Q_PROPERTY(bool value READ value WRITE setValue);    

public: 
    QLed(QWidget *parent = 0);
    ~QLed();
    bool value() const { return m_value; }
    enum ledColor { Red=0,Grey };
    enum ledShape { Square=0 };
    
public slots:
	void setValue(bool);     
	void toggleValue();

protected:
    bool m_value;
    ledColor m_onColor, m_offColor;
    int id_Timer;
    ledShape m_shape;
    QStringList shapes;
    QStringList colors;
    void paintEvent(QPaintEvent *event);
    QLabel *label;
};
#endif
