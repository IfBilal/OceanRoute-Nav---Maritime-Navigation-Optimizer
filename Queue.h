#pragma once
#include <iostream>
#include "LinkedList.h"

using namespace std;

template <typename T>
class Queue
{
private:
    LinkedList<T> list;

public:
    void enqueue(T data)
    {
        list.insertAtEnd(data);
    }
    void dequeue()
    {
        list.popAtHead();
    }
    Queue &operator=(const Queue<T> &other)
    {
        if (this != &other)
        {
            list = other.list;
        }
        return *this;
    }
    T &front()
    {
        return list.getHead();
    }
    
    const T &front() const
    {
        return list.getHead();
    }
    
    bool isEmpty()
    {
        return list.empty();
    }
};