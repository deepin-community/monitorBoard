#pragma once
#include <QDir>
#include <QString>
typedef struct
{
    int id;
    int mem;
    int cpu;
    QString name;
} process;
class Process
{
  public:
    int id;
    int mem;
    int cpu;
    QString name;

    Process();
    Process(int id, int mem, int cpu, QString name);
    ~Process();
};

class ProcessNode
{
  public:
    Process *process;
    ProcessNode *pre;
    ProcessNode *next;

    ProcessNode();
    ProcessNode(int id, int mem, int cpu, QString name);
    ~ProcessNode();
    void append(ProcessNode *obj);
    void insert(ProcessNode *add, ProcessNode *obj);
    void remove(ProcessNode *obj);
};
