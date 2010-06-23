#ifndef CGBASIMMAINWINDOW_H
#define CGBASIMMAINWINDOW_H

#include <QtGui>
#include "cgbaSim.h"

class CgbaSimMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CgbaSimMainWindow();
    ~CgbaSimMainWindow();
public slots:
    void quit();
    void about();
protected:
    void setupWindow();
    void createActions();
    void createMenus();
private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *exitAction;
    QAction *aboutAction;

    cgbaSim *mainWidget;
};

#endif // CGBASIMMAINWINDOW_H
