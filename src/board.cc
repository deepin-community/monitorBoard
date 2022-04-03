#include "board.hh"
#include "ui_board.h"
#include <QFileInfo>

Board::Board(QWidget *parent) : QWidget(parent), ui(new Ui::Board)
{
    ui->setupUi(this);
    init();
}

Board::~Board()
{
    delete ui;
}

//获取进程名
QString Board::getName(QString pid)
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

//获取用户
QString Board::getUser(QString pid)
{
    return QFileInfo(QFile::symLinkTarget("/proc/" + pid + "/cwd")).baseName();
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

    this->rByte = 0;
    this->wByte = 0;
    //初始化获取信息
    getMem();     //总内存
    getProcess(); //进程列表
    getCPU();     // CPU总时间和空闲时间
    //开启定时器
    timer.start(1000);
    connect(&timer, &QTimer::timeout, this, &Board::refresh);
}

//获取总CPU信息
void Board::getCPU()
{
    QFile stat("/proc/stat");
    stat.open(QIODevice::ReadOnly | QIODevice::Text);

    QString stats = stat.readLine();
    stat.close();
    QStringList statsList = stats.split(' ');

    int cpu_id = statsList[5].toInt();
    int cpu = statsList[2].toInt() + statsList[3].toInt() + statsList[4].toInt() + statsList[5].toInt() +
              statsList[6].toInt() + statsList[7].toInt() + statsList[8].toInt();
    ui->cpuPercentLabel->setNum((100 - (100 * (cpu_id - this->cpu_id)) / (cpu - this->cpu)));
    this->cpu = cpu;
    this->cpu_id = cpu_id;
}

//获取总内存信息
void Board::getMem()
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

    ui->memPercentLabel->setNum((((this->memTotal - this->memAvailable) * 100) / this->memTotal));
}

//获取进程内存信息
float Board::getMem(QString pid)
{
    QFile memInfo("/proc/" + pid + "/status");
    memInfo.open(QIODevice::ReadOnly | QIODevice::Text);
    QString memStr;
    QStringList memStrList;
    memStr = memInfo.readAll();
    memInfo.close();
    memStrList = memStr.split('\n');
    memStrList[22].remove(' ');
    memStrList[22].remove("RssAnon:\t");
    memStrList[22].remove("kB");
    return (memStrList[22].toFloat() / 1024);
}

//获取进程I/O信息
int Board::getRByte(QString pid)
{
    QFile ioInfo("/proc/" + pid + "/io");
    ioInfo.open(QIODevice::ReadOnly | QIODevice::Text);
    QString ioInfos = ioInfo.readAll();
    ioInfo.close();
    if (ioInfos.isEmpty())
    {
        return 0;
    }
    QStringList ioInfosList = ioInfos.split('\n');
    ioInfosList[4].remove(' ');
    ioInfosList[4].remove("read_bytes:");
    this->rByte += (ioInfosList[4].toInt() / 1024) / 1024;
    return (ioInfosList[4].toInt() / 1048576);
}

int Board::getWByte(QString pid)
{
    QFile ioInfo("/proc/" + pid + "/io");
    ioInfo.open(QIODevice::ReadOnly | QIODevice::Text);
    QString ioInfos = ioInfo.readAll();
    ioInfo.close();
    if (ioInfos.isEmpty())
    {
        return 0;
    }
    QStringList ioInfosList = ioInfos.split('\n');
    ioInfosList[5].remove(' ');
    ioInfosList[5].remove("write_bytes:");
    this->wByte += (ioInfosList[4].toInt() / 1024) / 1024;
    return (ioInfosList[4].toInt() / 1048576);
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
    QStringList dirs = proc.entryList(QDir::Dirs, QDir::Unsorted);

    //设置表格大小
    ui->tableProcess->setRowCount(dirs.length());
    ui->processNum->setNum(dirs.length());
    //插入时禁止排序
    ui->tableProcess->setSortingEnabled(false);
    for (const QString &dir : dirs)
    {
        //构造进程对象
        ProcessNode *obj =
            new ProcessNode(dir.toInt(), getMem(dir), 0, getRByte(dir), getWByte(dir), getName(dir), getUser(dir));
        //插入进程
        processes->append(obj);
        //填入表格
        ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
        ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(obj->process->id)));
        ui->tableProcess->setItem(dirs.indexOf(dir), 2, new QTableWidgetItem(obj->process->user));
        ui->tableProcess->setItem(dirs.indexOf(dir), 4,
                                  new QTableWidgetItem(QString::number(obj->process->mem, 'f', 2) + "M"));
        ui->tableProcess->setItem(dirs.indexOf(dir), 5, new QTableWidgetItem(QString::number(obj->process->rByte)));
        ui->tableProcess->setItem(dirs.indexOf(dir), 6, new QTableWidgetItem(QString::number(obj->process->wByte)));
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
    QStringList dirs = proc.entryList(QDir::Dirs, QDir::Unsorted);
    ui->tableProcess->setRowCount(dirs.length());
    ui->processNum->setNum(dirs.length());

    ui->tableProcess->setSortingEnabled(false);
    ProcessNode *p = this->processes;
    for (const QString &dir : dirs)
    {
        if (p->next == processes->pre)
        {
            ProcessNode *obj =
                new ProcessNode(dir.toInt(), getMem(dir), 0, getRByte(dir), getWByte(dir), getName(dir), getUser(dir));
            processes->append(obj);
            ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
            ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(obj->process->id)));
            ui->tableProcess->setItem(dirs.indexOf(dir), 2, new QTableWidgetItem(obj->process->user));
            ui->tableProcess->setItem(dirs.indexOf(dir), 4,
                                      new QTableWidgetItem(QString::number(obj->process->mem, 'f', 2) + "M"));
            ui->tableProcess->setItem(dirs.indexOf(dir), 5, new QTableWidgetItem(QString::number(obj->process->rByte)));
            ui->tableProcess->setItem(dirs.indexOf(dir), 6, new QTableWidgetItem(QString::number(obj->process->wByte)));
            p = p->next;
        }
        else
        {
            p = p->next;
            if (p->process->id == dir.toInt())
            {
                p->process->rByte = getRByte(dir);
                p->process->wByte = getWByte(dir);
                p->process->mem = getMem(dir);
                ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(p->process->name));
                ui->tableProcess->setItem(dirs.indexOf(dir), 1, new QTableWidgetItem(QString::number(p->process->id)));
                ui->tableProcess->setItem(dirs.indexOf(dir), 2, new QTableWidgetItem(p->process->user));
                ui->tableProcess->setItem(dirs.indexOf(dir), 4,
                                          new QTableWidgetItem(QString::number(p->process->mem, 'f', 2) + "M"));
                ui->tableProcess->setItem(dirs.indexOf(dir), 5,
                                          new QTableWidgetItem(QString::number(p->process->rByte)));
                ui->tableProcess->setItem(dirs.indexOf(dir), 6,
                                          new QTableWidgetItem(QString::number(p->process->wByte)));
                continue;
            }
            else
            {
                if (p->process->id > dir.toInt())
                {
                    p = p->pre;
                    ProcessNode *obj = new ProcessNode(dir.toInt(), getMem(dir), 0, getRByte(dir), getWByte(dir),
                                                       getName(dir), getUser(dir));
                    processes->insert(p, obj);
                    ui->tableProcess->setItem(dirs.indexOf(dir), 0, new QTableWidgetItem(obj->process->name));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 1,
                                              new QTableWidgetItem(QString::number(obj->process->id)));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 2, new QTableWidgetItem(obj->process->user));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 4,
                                              new QTableWidgetItem(QString::number(obj->process->mem, 'f', 2) + "M"));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 5,
                                              new QTableWidgetItem(QString::number(obj->process->rByte)));
                    ui->tableProcess->setItem(dirs.indexOf(dir), 6,
                                              new QTableWidgetItem(QString::number(obj->process->wByte)));
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
    ui->readLavel->setNum(this->rByte);
    ui->writeLabel->setNum(this->wByte);
    this->rByte = 0;
    this->wByte = 0;
    //允许排序
    ui->tableProcess->setSortingEnabled(true);
}

//刷新所有信息
void Board::refresh()
{
    getMem();
    getCPU();
    refreshProcess();
    timer.start(1000);
}
