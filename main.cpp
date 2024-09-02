#include "bluecheese.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bluecheese w;
    w.setWindowIcon(QIcon("resources/icon.ico"));
    w.show();
    return a.exec();
}
