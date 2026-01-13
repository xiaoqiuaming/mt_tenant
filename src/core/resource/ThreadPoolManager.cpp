#include "core/resource/ThreadPoolManager.h"
#include <numeric>

namespace yao {

ThreadPoolManager& ThreadPoolManager::getInstance() {
    static ThreadPoolManager instance;
    return instance;
}

bool ThreadPoolManager::initialize(size_t totalThreads, bool enableCgroup) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        return true;
    }

    totalThreads_ = totalThreads;
    cgroupEnabled_ = enableCgroup;

    if (cgroupEnabled_) {
        cgroupController_ = std::make_unique<CgroupController>();
        if (!cgroupController_->initialize()) {
            std::cerr << "Failed to initialize cgroup controller" << std::endl;
            return false;
        }
    }

    initialized_ = true;
    std::cout << "ThreadPoolManager initialized with " << totalThreads << " threads"
              << (cgroupEnabled_ ? " (cgroup enabled)" : "") << std::endl;

    return true;
}

void ThreadPoolManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) return;

    // 停止所有租户线程组
    for (auto& [tenantId, group] : tenantGroups_) {
        group->stop();
    }
    tenantGroups_.clear();

    cgroupController_.reset();
    initialized_ = false;

    std::cout << "ThreadPoolManager shutdown" << std::endl;
}

bool ThreadPoolManager::createTenantThreadGroup(const std::string& tenantId, size_t threadCount) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) return false;

    if (tenantGroups_.find(tenantId) != tenantGroups_.end()) {
        return false; // 已存在
    }

    // 检查总线程数限制
    size_t currentAllocated = std::accumulate(
        tenantGroups_.begin(), tenantGroups_.end(), 0UL,
        [](size_t sum, const auto& pair) {
            return sum + pair.second->getTotalThreads();
        });

    if (currentAllocated + threadCount > totalThreads_) {
        std::cerr << "Insufficient threads: requested " << threadCount
                  << ", available " << (totalThreads_ - currentAllocated) << std::endl;
        return false;
    }

    // 创建cgroup（如果启用）
    CgroupController* cgroup = nullptr;
    if (cgroupEnabled_) {
        if (!cgroupController_->createTenantCgroup(tenantId)) {
            std::cerr << "Failed to create cgroup for tenant " << tenantId << std::endl;
            return false;
        }
        cgroup = cgroupController_.get();
    }

    // 创建线程组
    auto group = std::make_unique<TenantThreadGroup>(tenantId, threadCount, cgroup);
    if (!group->start()) {
        std::cerr << "Failed to start thread group for tenant " << tenantId << std::endl;
        if (cgroupEnabled_) {
            cgroupController_->removeTenantCgroup(tenantId);
        }
        return false;
    }

    tenantGroups_[tenantId] = std::move(group);

    std::cout << "Created thread group for tenant " << tenantId
              << " with " << threadCount << " threads" << std::endl;

    return true;
}

bool ThreadPoolManager::removeTenantThreadGroup(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = tenantGroups_.find(tenantId);
    if (it == tenantGroups_.end()) {
        return false;
    }

    it->second->stop();
    tenantGroups_.erase(it);

    // 清理cgroup
    if (cgroupEnabled_) {
        cgroupController_->removeTenantCgroup(tenantId);
    }

    std::cout << "Removed thread group for tenant " << tenantId << std::endl;

    return true;
}

bool ThreadPoolManager::resizeTenantThreads(const std::string& tenantId, size_t newThreadCount) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = tenantGroups_.find(tenantId);
    if (it == tenantGroups_.end()) {
        return false;
    }

    // 检查总线程数限制
    size_t currentAllocated = std::accumulate(
        tenantGroups_.begin(), tenantGroups_.end(), 0UL,
        [](size_t sum, const auto& pair) {
            if (pair.first != tenantId) {
                return sum + pair.second->getTotalThreads();
            }
            return sum;
        });

    if (currentAllocated + newThreadCount > totalThreads_) {
        std::cerr << "Insufficient threads for resize: requested " << newThreadCount
                  << ", available " << (totalThreads_ - currentAllocated) << std::endl;
        return false;
    }

    return it->second->resize(newThreadCount);
}

bool ThreadPoolManager::submitTask(const std::string& tenantId, std::unique_ptr<Task> task) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = tenantGroups_.find(tenantId);
    if (it == tenantGroups_.end()) {
        return false;
    }

    return it->second->submitTask(std::move(task));
}

ThreadPoolManager::ThreadGroupInfo ThreadPoolManager::getTenantThreadInfo(const std::string& tenantId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    ThreadGroupInfo info;

    auto it = tenantGroups_.find(tenantId);
    if (it != tenantGroups_.end()) {
        info.totalThreads = it->second->getTotalThreads();
        info.busyThreads = it->second->getBusyThreads();
        info.queueSize = it->second->getQueueSize();
    }

    return info;
}

ThreadPoolManager::SystemThreadInfo ThreadPoolManager::getSystemThreadInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);

    SystemThreadInfo info;
    info.totalThreads = totalThreads_;

    info.allocatedThreads = std::accumulate(
        tenantGroups_.begin(), tenantGroups_.end(), 0UL,
        [](size_t sum, const auto& pair) {
            return sum + pair.second->getTotalThreads();
        });

    // 系统线程数（预留给系统任务）
    info.systemThreads = totalThreads_ - info.allocatedThreads;

    return info;
}

} // namespace yao