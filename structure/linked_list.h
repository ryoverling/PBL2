#pragma once

#include <cstddef>
#include "node.h"
using namespace std;

template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t size;

public:
    LinkedList() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

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
                if (previous == nullptr) {
                    head = current->next;
                } else {
                    previous->next = current->next;
                }

                if (current == tail) {
                    tail = previous;
                }

                delete current;
                size--;
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

    template <typename Func>
    void forEach(Func func) {
        Node<T>* current = head;
        while (current != nullptr) {
            func(current->data);
            current = current->next;
        }
    }

    template <typename Func>
    void forEach(Func func) const {
        Node<T>* current = head;
        while (current != nullptr) {
            func(current->data);
            current = current->next;
        }
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
