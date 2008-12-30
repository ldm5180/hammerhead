
//
// Copyright (C) 2008, Regents of the University of Colorado.
//


#include <QApplication>

#include <mainwindow.h>

using namespace std;


int main(int argc, char* argv[]) {
    QApplication bionetMonitor(argc, argv);

    MainWindow* window = new MainWindow(argv);

    window->resize(900, 400);
    window->show();

    return bionetMonitor.exec();
}
