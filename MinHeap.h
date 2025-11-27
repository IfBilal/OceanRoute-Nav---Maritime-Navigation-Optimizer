#pragma once
#include "Vector.h"
template <typename T>
class MinHeap
{
private:
    vector<T> arr;

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
        arr[0] = arr[arr.getSize() - 1];
        arr.pop_back();
        heapifyDown(0);
    }
    T top()
    {
        return arr[0];
    }
    bool isEmpty()
    {
        return arr.getSize() == 0;
    }
};