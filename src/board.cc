#include "board.hh"
#include "ui_board.h"
#include <QFileInfo>
//获取进程名
QString getName(QString pid)
{
    // QFile stat("/proc/" + pid + "/stat");
    return QFileInfo(QFile::symLinkTarget("/proc/" + pid + "/exe")).baseName();
    // stat.open(QIODevice::ReadOnly | QIODevice::Text);
    // QString stats = stat.readAll();
    // stat.close();
    // QStringList statList = stats.split(" ");
    // statList[1].remove("(");
    // statList[1].remove(")");
    // return statList[1];
}

Board::Board(QWidget *parent) : QWidget(parent), ui(new Ui::Board)
{
    ui->setupUi(this);
    init();
}

Board::~Board()
{
    delete ui;
}

//初始化
void Board::init()
{
    //初始化对象
    processes = new ProcessNode;
    //获取设备总内存
    QFile memInfo("/proc/meminfo");
    memInfo.open(QIODevice::ReadOnly | QIODevice::Text);
    QString memStr;
    QStringList memStrList;
    memStr = memInfo.readAll();
    memInfo.close();
    memStrList = memStr.split('\n');
    memStrList[0].remove(' ');
    memStrList[0].remove("MemTotal:");
    memStrList[0].remove("kB");
    this->memTotal = memStrList[0].toInt();
    //初始化获取信息
    setMem();     //总内存
    getProcess(); //进程列表
    //开启定时器
    timer.start(1000);
    connect(&timer, &QTimer::timeout, this, &Board::refresh);
}

//获取总内存信息
int Board::getMem()
{
    QFile memInfo("/proc/meminfo");
    memInfo.open(QIODevice::ReadOnly | QIODevice::Text);

    QString memStr;
    QStringList memStrList;
    memStr = memInfo.readAll();
    memInfo.close();
    memStrList = memStr.split('\n');
    memStrList[2].remove(' ');
    memStrList[2].remove("MemAvailable:");
    memStrList[2].remove("kB");
    this->memAvailable = memStrList[2].toInt();

    return (((this->memTotal - this->memAvailable) * 100) / this->memTotal);
}

//刷新内存信息
void Board::setMem()
{
    ui->memPercentLabel->setNum(getMem());
}

//获取进程列表
void Board::getProcess()
{
    //设置要遍历的目录
    QDir proc("/proc");
    //过滤无关目录 只保留数字开头的目录
    QStringList filters;
    filters << "[1-9]*";
    proc.setNameFilters(filters);
    //获取需要遍历的目录列表
    QStringList dirs = proc.entryList(QDir::Dirs, QDir::Name);
    //设置表格大小
    ui->tableProcess->setRowCount(dirs.length());
    ui->processNum->setNum(dirs.length());
    //插入时禁止排序
    ui->tableProcess->setSortingEnabled(false);
    for (const QString &dir : dirs)
    {
        //构造进程对象
        ProcessNode *obj = new ProcessNode(dir.toInt(), 0, 0, getName(dir));
        //插入进程
        processes->append(obj);
        //填入表格
        ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
        ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(obj->process->id)));
    }
    //允许排序
    ui->tableProcess->setSortingEnabled(true);
}

//刷新进程列表
void Board::refreshProcess()
{
    QDir proc("/proc");
    QStringList filters;
    filters << "[1-9]*";
    proc.setNameFilters(filters);
    QStringList dirs = proc.entryList(QDir::Dirs, QDir::Name);
    ui->tableProcess->setRowCount(dirs.length());
    ui->processNum->setNum(dirs.length());
    ui->tableProcess->setSortingEnabled(false);
    ProcessNode *p = this->processes;
    for (const QString &dir : dirs)
    {
        if (p->next == nullptr)
        {
            ProcessNode *obj = new ProcessNode(dir.toInt(), 0, 0, getName(dir));
            processes->append(obj);
            ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
            ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(obj->process->id)));
            p = p->next;
        }
        else
        {
            p = p->next;
            if (p->process->id == dir.toInt())
            {
                ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(p->process->name));
                ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(p->process->id)));
                continue;
            }
            else
            {
                if (p->process->id > dir.toInt())
                {
                    p = p->pre;
                    ProcessNode *obj = new ProcessNode(dir.toInt(), 0, 0, getName(dir));
                    processes->insert(p, obj);
                    ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 1,
                                              new QTableWidgetItem(QString::number(obj->process->id)));
                    p = p->next;
                }
                else
                {
                    p = p->pre;
                    processes->remove(p->next);
                }
            }
        }
    }
    //允许排序
    ui->tableProcess->setSortingEnabled(true);
}

//刷新所有信息
void Board::refresh()
{
    setMem();
    refreshProcess();
    timer.start(1000);
}
