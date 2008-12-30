
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#ifndef NODE_H
#define NODE_H

#include <QAbstractItemView>
#include <QList>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QRegExp>
#include <QString>
#include <QWidget>

#include <iostream>

extern "C" {
#include "bionet.h"
}

using namespace std;

enum EndpointTypeT {
    PRODUCER,
    CONSUMER,
    NEITHER
};

enum BionetTypeT {
    HAB,
    NODE,
    STREAM,
    ROOT
};

class Node : public QWidget {
    Q_OBJECT

public:
    Node(QString name, QWidget* parent = 0);
    ~Node();

    void addChild(Node* child);
    void addChildWithoutArea(Node* child);
    bool removeChild(QString id);
    Node* find(QString fullname);
    Node* find(QPoint point);
    Node* find(QRect rect);

    // Get Functions
    QRect getArea() const { return area; }
    QString getId () const { return id; }
    QString getName () const { return name; }
    BionetTypeT getBionetType () const { return bionetType; }
    EndpointTypeT getEndpointType () const { return endpointType; }
    QList<Node*>* getChildren() const { return children; }
    void* getBionetPtr() const { return bionetPtr; }

    void setArea(QRect newArea);

private:
    QRect area;
    QString id, name;
    EndpointTypeT endpointType;
    BionetTypeT bionetType;
    QList<Node*>* children;
    void* bionetPtr;
};

#endif
