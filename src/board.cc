#include "board.hh"
#include "ui_board.h"

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
    timer.start(1000);
    connect(&timer, &QTimer::timeout, this, &Board::refresh);
    processes = new (ProcessList);
    refreshList();
}

//获取内存信息
int Board::getMem()
{
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
void Board::refreshList()
{
    //设置要遍历的目录
    QDir dir("/proc");
    // QStringList filters;
    // filters << "\"[1-9]\\d*\"";
    // dir.setNameFilters(filters);
    QStringList dirs = dir.entryList(QDir::Dirs, QDir::Name);
    for (int i = 0; i < dirs.length(); i++)
    {
        node *obj = (node *)malloc(sizeof(node));
        obj->process.id = dirs[i].toInt();
        processes->append(obj);
        // ui->comboBox->addItem(obj->process.id);
    }
}
void Board::refresh()
{
    setMem();
    timer.start(1000);
}
