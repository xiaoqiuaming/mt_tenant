#include "LockFreeQueue.h"
#include <iostream>

namespace yao {

LockFreeQueue::LockFreeQueue(size_t capacity)
    : capacity_(capacity), size_(0) {
    Node* dummy = new Node();
    head_.store(dummy);
    tail_.store(dummy);
}

LockFreeQueue::~LockFreeQueue() {
    // 清理所有节点
    Node* current = head_.load();
    while (current) {
        Node* next = current->next.load();
        delete current;
        current = next;
    }
}

bool LockFreeQueue::enqueue(std::unique_ptr<Task> task) {
    if (!task) return false;

    Node* new_node = new Node(std::move(task));

    while (true) {
        Node* tail = tail_.load();
        Node* next = tail->next.load();

        if (tail == tail_.load()) {  // 检查tail是否仍然有效
            if (next == nullptr) {   // tail是真正的尾节点
                if (tail->next.compare_exchange_weak(next, new_node)) {
                    tail_.compare_exchange_weak(tail, new_node);
                    size_.fetch_add(1);
                    return true;
                }
            } else {
                // 帮助推进tail
                tail_.compare_exchange_weak(tail, next);
            }
        }
    }
}

std::unique_ptr<Task> LockFreeQueue::dequeue() {
    while (true) {
        Node* head = head_.load();
        Node* tail = tail_.load();
        Node* next = head->next.load();

        if (head == head_.load()) {  // 检查head是否仍然有效
            if (head == tail) {      // 队列为空或正在变化
                if (next == nullptr) {
                    return nullptr;  // 队列为空
                }
                // 帮助推进tail
                tail_.compare_exchange_weak(tail, next);
            } else {
                // 尝试移除head
                if (head_.compare_exchange_weak(head, next)) {
                    std::unique_ptr<Task> task = std::move(next->task);
                    size_.fetch_sub(1);
                    delete head;  // 删除旧的dummy节点
                    return task;
                }
            }
        }
    }
}

size_t LockFreeQueue::size() const {
    return size_.load();
}

bool LockFreeQueue::empty() const {
    return size_.load() == 0;
}

} // namespace yao