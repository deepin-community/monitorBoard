#pragma once
#include <QDir>
#include <QString>

class Process
{
  public:
    int id;
    float mem;
    int cpu;
    QString user;
    QString name;

    Process();
    Process(int id, float mem, int cpu, QString name, QString user);
    ~Process();
};

class ProcessNode
{
  public:
    Process *process;
    ProcessNode *pre;
    ProcessNode *next;

    ProcessNode();
    ProcessNode(int id, float mem, int cpu, QString name, QString user);
    ~ProcessNode();
    void append(ProcessNode *obj);
    void insert(ProcessNode *add, ProcessNode *obj);
    void remove(ProcessNode *obj);
};
