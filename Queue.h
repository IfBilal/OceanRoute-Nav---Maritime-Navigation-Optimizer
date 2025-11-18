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
    T& front()
    {
        return list.getHead();
    }
    bool isEmpty()
    {
        return list.empty();
    }
};