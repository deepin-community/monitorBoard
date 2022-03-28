#pragma once
#include <QDir>
#include <QString>
typedef struct
{
    int id;
    int mem;
    int pid;
    QString name;
} process;

typedef struct node
{
    process process;
    struct node *pre;
    struct node *next;
} node;

class ProcessList
{
  public:
    node *list;

    ProcessList();
    ~ProcessList();
    void append(node *obj);
    void initList();
    void insert(node *add, node *obj);
    void remove(node *obj);
};
