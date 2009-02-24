
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


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
