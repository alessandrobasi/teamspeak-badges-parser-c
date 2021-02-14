#include "TeamspeakBadgesViewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TeamspeakBadgesViewer w;
    w.show();
    return a.exec();
}
