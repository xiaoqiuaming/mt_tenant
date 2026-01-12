#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

namespace yao {

/**
 * @brief 任务接口
 */
class Task {
public:
    virtual ~Task() = default;
    virtual void execute() = 0;
    virtual bool isValid() const = 0;
};

/**
 * @brief 无锁任务队列
 * 基于CAS实现的多生产者多消费者无锁队列
 */
class LockFreeQueue {
public:
    LockFreeQueue(size_t capacity = 1024);
    ~LockFreeQueue();

    /**
     * @brief 入队操作
     * @param task 任务指针
     * @return 是否成功
     */
    bool enqueue(std::unique_ptr<Task> task);

    /**
     * @brief 出队操作
     * @return 任务指针，如果队列为空返回nullptr
     */
    std::unique_ptr<Task> dequeue();

    /**
     * @brief 获取队列大小
     * @return 当前队列中的任务数量
     */
    size_t size() const;

    /**
     * @brief 检查队列是否为空
     * @return 是否为空
     */
    bool empty() const;

private:
    struct Node {
        std::unique_ptr<Task> task;
        std::atomic<Node*> next;
        Node() : next(nullptr) {}
        explicit Node(std::unique_ptr<Task> t) : task(std::move(t)), next(nullptr) {}
    };

    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
    std::atomic<size_t> size_;
    const size_t capacity_;
};

} // namespace yao