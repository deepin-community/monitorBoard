#include "processlist.hh"
Process::Process(int id, int mem, int cpu, QString name)
{
    this->id = id;
    this->mem = mem;
    this->cpu = cpu;
    this->name = name;
}
ProcessNode::ProcessNode()
{
    this->process = new Process(0, 0, 0, "HEAD");
    this->pre = this;
    this->next = nullptr;
}
ProcessNode::ProcessNode(int id, int mem, int cpu, QString name)
{
    this->process = new Process(id, mem, cpu, name);
    this->next = nullptr;
}
ProcessNode::~ProcessNode()
{
    ProcessNode *p = nullptr;
    ProcessNode *pre = this;
    while (pre != nullptr)
    {
        p = pre->next;
        free(pre);
        pre = p;
    }
}

void ProcessNode::append(ProcessNode *obj)
{
    obj->pre = this->pre;
    obj->next = nullptr;
    this->pre->next = obj;
    this->pre = obj;
}
void ProcessNode::insert(ProcessNode *add, ProcessNode *obj)
{
    obj->pre = add;
    obj->next = add->next;
    add->next = obj;
    obj->next->pre = obj;
}
void ProcessNode::remove(ProcessNode *obj)
{
    obj->pre->next = obj->next;
    obj->next->pre = obj->pre;
    // free(obj);
}
