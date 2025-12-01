#pragma once
#include "Vector.h"
template <typename T>
class MinHeap
{
private:
    Vector<T> arr;

public:
    bool hasLeftChild(int i)
    {
        return 2 * i + 1 < arr.getSize();
    }
    bool hasRightChild(int i)
    {
        return 2 * i + 2 < arr.getSize();
    }
    int parent(int i)
    {
        return (i - 1) / 2;
    }
    int leftChild(int i)
    {
        return 2 * i + 1;
    }
    int rightChild(int i)
    {
        return 2 * i + 2;
    }

    void heapifyUp(int i)
    {
        while (i != 0 && arr[parent(i)] > arr[i])
        {
            swap(arr[parent(i)], arr[i]);
            i = parent(i);
        }
    }
    void heapifyDown(int i)
    {
        while (true)
        {
            int smallest = i;
            if (hasLeftChild(i) && arr[leftChild(i)] < arr[i])
                smallest = leftChild(i);
            if (hasRightChild(i) && arr[rightChild(i)] < arr[smallest])
                smallest = rightChild(i);
            if (smallest == i)
                break;
            swap(arr[i], arr[smallest]);
            i = smallest;
        }
    }
    void push(T value)
    {
        arr.push_back(value);
        heapifyUp(arr.getSize() - 1);
    }
    void pop()
    {
        if (isEmpty())
        {
            throw out_of_range("Cannot pop from empty heap");
        }
        arr[0] = arr[arr.getSize() - 1];
        arr.pop_back();
        if (!isEmpty())
        {
            heapifyDown(0);
        }
    }
    const T& top() const
    {
        if (isEmpty())
        {
            throw out_of_range("Cannot access top of empty heap");
        }
        return arr[0];
    }
    bool isEmpty() const
    {
        return arr.getSize() == 0;
    }
};