#include "board.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Board w;
    w.show();
    return a.exec();
}
