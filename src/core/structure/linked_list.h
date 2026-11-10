#pragma once

#include <cstddef>
#include <utility>
#include "node.h"

template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t size;

    void unlinkNode(Node<T>* current, Node<T>* previous) {
        if (previous == nullptr) head = current->next;
        else previous->next = current->next;
        if (current == tail) tail = previous;
        delete current;
        size--;
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    bool isEmpty() const {
        return size == 0;
    }

    size_t getSize() const {
        return size;
    }

    Node<T>* getHead() const {
        return head;
    }

    // Sắp xếp chọn (Selection Sort): đổi chỗ data giữa 2 node, không đổi con trỏ node
    template <typename Compare>
    void selectionSort(Compare comesBefore) {
        for (Node<T>* i = head; i != nullptr; i = i->next) {
            Node<T>* minNode = i;
            for (Node<T>* j = i->next; j != nullptr; j = j->next) {
                if (comesBefore(j->data, minNode->data)) {
                    minNode = j;
                }
            }
            if (minNode != i) {
                std::swap(i->data, minNode->data);
            }
        }
    }

    void pushBack(const T& value) {
        Node<T>* node = new Node<T>(value);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
    }

    template <typename Predicate>
    bool removeIf(Predicate predicate) {
        Node<T>* current = head;
        Node<T>* previous = nullptr;

        while (current != nullptr) {
            if (predicate(current->data)) {
                unlinkNode(current, previous);
                return true;
            }

            previous = current;
            current = current->next;
        }
        return false;
    }

    template <typename Predicate>
    T* findIf(Predicate predicate) {
        Node<T>* current = head;
        while (current != nullptr) {
            if (predicate(current->data)) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr;
    }

    template <typename Predicate>
    const T* findIf(Predicate predicate) const {
        Node<T>* current = head;
        while (current != nullptr) {
            if (predicate(current->data)) {
                return &current->data;
            }
            current = current->next;
        }
        return nullptr;
    }

    void clear() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = tail = nullptr;
        size = 0;
    }
};
