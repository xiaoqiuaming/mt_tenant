#pragma once

#include "core/resource/LockFreeQueue.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <chrono>

namespace yao {

class CgroupController;

/**
 * @brief 工作线程类
 */
class WorkerThread {
public:
    WorkerThread(const std::string& tenantId, LockFreeQueue& queue, CgroupController* cgroup = nullptr);
    ~WorkerThread();

    /**
     * @brief 启动线程
     */
    void start();

    /**
     * @brief 停止线程
     */
    void stop();

    /**
     * @brief 获取线程ID
     */
    std::thread::id getId() const;

    /**
     * @brief 检查线程是否忙碌
     */
    bool isBusy() const;

    /**
     * @brief 获取执行的任务数量
     */
    size_t getExecutedTasks() const;

private:
    void run();

    std::string tenantId_;
    LockFreeQueue& taskQueue_;
    CgroupController* cgroup_;
    std::unique_ptr<std::thread> thread_;
    std::atomic<bool> running_;
    std::atomic<bool> busy_;
    std::atomic<size_t> executedTasks_;
};

/**
 * @brief 租户线程组
 * 管理一组为特定租户工作的线程
 */
class TenantThreadGroup {
public:
    TenantThreadGroup(const std::string& tenantId, size_t threadCount, CgroupController* cgroup = nullptr);
    ~TenantThreadGroup();

    /**
     * @brief 启动线程组
     */
    bool start();

    /**
     * @brief 停止线程组
     */
    void stop();

    /**
     * @brief 提交任务到队列
     */
    bool submitTask(std::unique_ptr<Task> task);

    /**
     * @brief 获取队列大小
     */
    size_t getQueueSize() const;

    /**
     * @brief 获取忙碌线程数
     */
    size_t getBusyThreads() const;

    /**
     * @brief 获取总线程数
     */
    size_t getTotalThreads() const;

    /**
     * @brief 调整线程数量
     */
    bool resize(size_t newThreadCount);

private:
    std::string tenantId_;
    std::vector<std::unique_ptr<WorkerThread>> threads_;
    LockFreeQueue taskQueue_;
    CgroupController* cgroup_;
    std::atomic<bool> running_;
};

} // namespace yao