#include "processlist.hh"

ProcessList::ProcessList()
{
    initList();
}
ProcessList::~ProcessList()
{
    node *p = nullptr;
    node *pre = this->list;
    while (pre != nullptr)
    {
        p = pre->next;
        free(pre);
        pre = p;
    }
}
void ProcessList::initList()
{
    this->list = (node *)malloc(sizeof(node));
    list->next = nullptr;
    list->pre = list;
}
void ProcessList::append(node *obj)
{
    this->list->pre->next = obj;
    obj->pre = this->list->pre->next;
    obj->next = nullptr;
}
void ProcessList::insert(node *add, node *obj)
{
    obj->pre = add;
    obj->next = add->next;
    add->next = obj;
}
void ProcessList::remove(node *obj)
{
    obj->pre->next = obj->next;
    obj->next->pre = obj->pre;
    free(obj);
}
