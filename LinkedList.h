#pragma once
#include <iostream>
using namespace std;
template <typename T>
struct Node
{
    T data;
    Node *next;

    Node(T data) : data(data), next(nullptr) {}
};
template <typename T>
struct LinkedList
{
    Node<T> *head;

    LinkedList()
    {
        head = nullptr;
    }

    void clear()
    {
        Node<T> *current = head;
        while (current != nullptr)
        {
            Node<T> *next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }
    LinkedList(const LinkedList<T> &other)
    {
        head = nullptr;
        Node<T> *current = other.head;
        while (current != nullptr)
        {
            insertAtEnd(current->data);
            current = current->next;
        }
    }
    LinkedList &operator=(const LinkedList<T> &other)
    {
        if (this != &other)
        {
            clear();
            Node<T> *current = other.head;
            while (current != nullptr)
            {
                insertAtEnd(current->data);
                current = current->next;
            }
        }
        return *this;
    }
    bool empty()
    {
        return head == nullptr;
    }
    void insertAtHead(T data)
    {
        Node<T> *newNode = new Node<T>(data);
        newNode->next = head;
        head = newNode;
    }

    T &getHead()
    {
        if (!head)
        {
            throw out_of_range("Index out of range");
        }
        return head->data;
    }
    
    const T &getHead() const
    {
        if (!head)
        {
            throw out_of_range("Index out of range");
        }
        return head->data;
    }
    void popAtHead()
    {
        if (head != nullptr)
        {
            Node<T> *temp = head;
            head = head->next;
            delete temp;
        }
    }
    void insertAtEnd(T data)
    {
        Node<T> *newNode = new Node<T>(data);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            Node<T> *current = head;
            while (current->next != nullptr)
            {
                current = current->next;
            }
            current->next = newNode;
        }
    }
    void popAtEnd()
    {
        if (head == nullptr)
        {
            return;
        }
        else if (head->next == nullptr)
        {
            delete head;
            head = nullptr;
        }
        else
        {
            Node<T> *current = head;
            while (current->next->next != nullptr)
            {
                current = current->next;
            }
            delete current->next;
            current->next = nullptr;
        }
    }
    ~LinkedList()
    {
        clear();
    }
};