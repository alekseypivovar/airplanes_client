#include "Menu.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Menu w;
 //   w.showFullScreen();
    w.show();

    return a.exec();
}
