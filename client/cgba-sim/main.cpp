#include "cgbasimmainwindow.h"

int main(int argc, char* argv[])
{
    QApplication CgbaSimulator(argc, argv);

    CgbaSimMainWindow sim;
    sim.show();

    return CgbaSimulator.exec();
}
