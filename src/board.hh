#pragma once

#include <QFile>
#include <QTimer>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class Board;
}
QT_END_NAMESPACE

class Board : public QWidget
{
    Q_OBJECT

  public:
    int memAvailable; //可用内存
    int memTotal;     //总内存
    int cpu;
    QTimer timer; //定时器

    Board(QWidget *parent = nullptr);
    ~Board();

    int getMem();
    void memRefresh();
    void init();

  private:
    Ui::Board *ui;
};
