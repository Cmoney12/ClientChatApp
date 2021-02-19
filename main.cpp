#include "login.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    login l;
    l.resize(500,500);
    l.show();
    return QApplication::exec();
}