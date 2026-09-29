#pragma once

template <typename T>
struct Node {
    T data;
    Node<T>* next;

    Node(const T& data) {
        this->data = data;
        next = nullptr;
    }
};
