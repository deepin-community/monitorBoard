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
    connect(&timer, &QTimer::timeout, this, &Board::memRefresh);
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
void Board::memRefresh()
{
    ui->memPercentLabel->setNum(getMem());
    timer.start(1000);
}
