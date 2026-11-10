#pragma once

template <typename T>
struct Node {
    T data;
    Node<T>* next;

    explicit Node(const T& data) : data(data), next(nullptr) {}
};
