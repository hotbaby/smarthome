#ifndef _DEVICE_TREE_H
#define _DVICE_TREE_H

#include "jsonrpcHanderl.h"

template<typename T>
class Tree
{
public:
    
    Tree(T *obj)
    :m_obj(obj), parent(0), child(0), sibling(0)
    {}

    ~Tree() {}
    
    void add (Tree<T> *node)
    {
        if(m_child)
        {
            Tree *tmp = m_child;
            m_child = node;
            node->m_parent = this;
            node->m_sibling = tmp;
        }
        else
        {
            m_child = node;
            node->m_parent = this;
        }
    }

    void delete(Tree<T> *tree)
    {
    }

private:
    Tree *m_parent;
    Tree *m_child;
    Tree *m_sibling;

    T * m_obj;
};
#endif
