#pragma once

#include "TenantThreadGroup.h"
#include "CgroupController.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <algorithm>
#include <iostream>

namespace yao {

/**
 * @brief 线程池管理器
 * 管理全局线程池和租户线程组分配
 */
class ThreadPoolManager {
public:
    static ThreadPoolManager& getInstance();

    /**
     * @brief 初始化线程池
     * @param totalThreads 总线程数（默认120）
     * @param enableCgroup 是否启用cgroup
     * @return 是否成功
     */
    bool initialize(size_t totalThreads = 120, bool enableCgroup = false);

    /**
     * @brief 关闭线程池
     */
    void shutdown();

    /**
     * @brief 创建租户线程组
     * @param tenantId 租户ID
     * @param threadCount 分配的线程数
     * @return 是否成功
     */
    bool createTenantThreadGroup(const std::string& tenantId, size_t threadCount);

    /**
     * @brief 删除租户线程组
     * @param tenantId 租户ID
     * @return 是否成功
     */
    bool removeTenantThreadGroup(const std::string& tenantId);

    /**
     * @brief 调整租户线程数
     * @param tenantId 租户ID
     * @param newThreadCount 新的线程数
     * @return 是否成功
     */
    bool resizeTenantThreads(const std::string& tenantId, size_t newThreadCount);

    /**
     * @brief 提交任务到租户队列
     * @param tenantId 租户ID
     * @param task 任务
     * @return 是否成功
     */
    bool submitTask(const std::string& tenantId, std::unique_ptr<Task> task);

    /**
     * @brief 获取租户线程组信息
     * @param tenantId 租户ID
     * @return 线程组信息
     */
    struct ThreadGroupInfo {
        size_t totalThreads = 0;
        size_t busyThreads = 0;
        size_t queueSize = 0;
    };
    ThreadGroupInfo getTenantThreadInfo(const std::string& tenantId) const;

    /**
     * @brief 获取系统线程信息
     */
    struct SystemThreadInfo {
        size_t totalThreads = 0;
        size_t allocatedThreads = 0;
        size_t systemThreads = 0;
    };
    SystemThreadInfo getSystemThreadInfo() const;

private:
    ThreadPoolManager() = default;
    ~ThreadPoolManager() { shutdown(); }

    ThreadPoolManager(const ThreadPoolManager&) = delete;
    ThreadPoolManager& operator=(const ThreadPoolManager&) = delete;

    mutable std::mutex mutex_;
    size_t totalThreads_ = 0;
    bool initialized_ = false;
    bool cgroupEnabled_ = false;
    std::unique_ptr<CgroupController> cgroupController_;
    std::unordered_map<std::string, std::unique_ptr<TenantThreadGroup>> tenantGroups_;
};

} // namespace yao