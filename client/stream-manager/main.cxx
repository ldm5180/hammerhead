
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <QApplication>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    MainWindow *mw = new MainWindow(argv);
    mw->show();

    return a.exec();
}
