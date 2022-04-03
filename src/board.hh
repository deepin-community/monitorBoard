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
    int memAvailable;       //可用内存
    int memTotal;           //总内存
    int cpu;                // cpu总时间
    int cpu_id;             // cpu空闲时间
    int rByte;              //读字节数
    int wByte;              //写字节数
    ProcessNode *processes; //进程头节点

    QTimer timer; //定时器

    Board(QWidget *parent = nullptr);
    ~Board();

    void init();                  //初始化
    void getMem();                //获取总的内存信息
    void getCPU();                //获取CPU占用率信息
    void getProcess();            //第一次获取进程列表
    void refreshProcess();        //刷新进程列表
    QString getName(QString pid); //获取进程名
    QString getUser(QString pid); //获取用户名
    float getMem(QString pid);    //获取进程内存信息
    void refresh();               //刷新所有信息
    int getRByte(QString pid);    //获取进程读速率
    int getWByte(QString pid);    //获取进程写速率

  private:
    Ui::Board *ui;
};
