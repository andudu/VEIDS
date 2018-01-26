#include "VEIDS.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VEIDS w;
    w.show();

    return a.exec();
}
