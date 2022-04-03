#pragma once

#include "processlist.hh"
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
    ProcessNode *processes; //进程头节点

    QTimer timer; //定时器

    Board(QWidget *parent = nullptr);
    ~Board();

    void getProcess();     //第一次获取进程列表
    void refreshProcess(); //刷新进程列表
    int getMem();          //获取总的内存信息
    void setMem();         //设置内存信息
    void refresh();        //刷新所有信息
    void init();           //初始化

  private:
    Ui::Board *ui;
};
