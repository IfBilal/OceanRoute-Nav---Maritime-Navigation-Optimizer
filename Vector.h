#pragma once
#include <iostream>
using namespace std;
template <typename T>
class Vector
{
private:
    T *arr;
    int capacity;
    int size;

public:
    Vector(int c = 1)
    {
        size = 0;
        capacity = c;
        arr = new T[capacity];
    }

    Vector(const Vector<T> &other)
    {
        size = other.size;
        capacity = other.capacity;
        arr = new T[capacity];
        for (int i = 0; i < size; i++)
        {
            arr[i] = other.arr[i];
        }
    }

    Vector<T> &operator=(const Vector<T> &other)
    {
        if (this != &other)
        {
            delete[] arr;
            size = other.size;
            capacity = other.capacity;
            arr = new T[capacity];
            for (int i = 0; i < size; i++)
            {
                arr[i] = other.arr[i];
            }
        }
        return *this;
    }

    void push_back(T value)
    {
        if (size == capacity)
        {
            capacity *= 2;
            T *newArr = new T[capacity];
            for (int i = 0; i < size; i++)
            {
                newArr[i] = arr[i];
            }
            delete[] arr;
            arr = newArr;
        }
        arr[size] = value;
        size++;
    }

    T &operator[](int index)
    {
        if (index >= size || index < 0)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }
    
    const T &operator[](int index) const
    {
        if (index >= size || index < 0)
        {
            throw out_of_range("Index out of range");
        }
        return arr[index];
    }

    void pop_back()
    {
        if (size == 0)
        {
            throw out_of_range("Index out of range");
        }
        size--;
    }

    int getSize() const
    {
        return size;
    }

    T &back()
    {
        if (size == 0)
            throw out_of_range("Vector is empty");
        return arr[size - 1];
    }

    bool empty() const
    {
        return size == 0;
    }

    int getCapacity() const
    {
        return capacity;
    }

    void clear()
    {
        size = 0;
        if (capacity > 1)
        {
            capacity = 1;
            T *newArr = new T[capacity];
            delete[] arr;
            arr = newArr;
        }
    }
    ~Vector()
    {
        delete[] arr;
    }
};
