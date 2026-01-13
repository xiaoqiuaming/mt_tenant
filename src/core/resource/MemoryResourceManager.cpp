#include "core/resource/MemoryResourceManager.h"
#include "core/tenant/TenantContext.h"
#include <iostream>
#include <algorithm>

namespace yao {

MemoryResourceManager& MemoryResourceManager::getInstance() {
    static MemoryResourceManager instance;
    return instance;
}

bool MemoryResourceManager::initialize(size_t totalMemoryMB) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalMemoryMB_ = totalMemoryMB;
    allocatedTotalMB_ = 0;
    tenantMemoryStats_.clear();
    std::cout << "MemoryResourceManager initialized with " << totalMemoryMB << " MB total memory" << std::endl;
    return true;
}

bool MemoryResourceManager::allocateMemoryResource(const std::shared_ptr<TenantContext>& tenant) {
    if (!tenant) {
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();
    std::lock_guard<std::mutex> lock(mutex_);

    // 检查是否已分配
    if (tenantMemoryStats_.find(tenantId) != tenantMemoryStats_.end()) {
        return true;  // 已分配
    }

    // 计算内存配额（基于CPU配额的比例）
    double cpuQuota = tenant->getCpuQuota();
    double memoryQuotaMB = (cpuQuota / 100.0) * totalMemoryMB_ * 0.8;  // 80%比例

    // 检查总分配是否超过限制
    if (allocatedTotalMB_ + memoryQuotaMB > totalMemoryMB_) {
        std::cerr << "Insufficient memory for tenant: " << tenantId << std::endl;
        return false;
    }

    // 分配内存资源
    tenantMemoryStats_.emplace(tenantId, MemoryStats{memoryQuotaMB, 0.0, 0.0, {0.0}});
    allocatedTotalMB_ += memoryQuotaMB;

    std::cout << "Allocated " << memoryQuotaMB << " MB memory for tenant: " << tenantId << std::endl;
    return true;
}

double MemoryResourceManager::getTenantMemoryUsage(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantMemoryStats_.find(tenantId);
    if (it == tenantMemoryStats_.end()) {
        return -1.0;  // 未分配
    }
    return it->second.usedMB / it->second.quotaMB;  // 返回使用率
}

void MemoryResourceManager::updateMemoryUsage(const std::string& tenantId, double usageMB) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantMemoryStats_.find(tenantId);
    if (it == tenantMemoryStats_.end()) {
        return;
    }

    it->second.usedMB = usageMB;
    it->second.peakUsage = std::max(it->second.peakUsage.load(), usageMB);
}

bool MemoryResourceManager::checkMemoryQuota(const std::string& tenantId, double requestedMB) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantMemoryStats_.find(tenantId);
    if (it == tenantMemoryStats_.end()) {
        return false;
    }

    double currentUsage = it->second.usedMB + requestedMB;
    return currentUsage <= it->second.quotaMB;
}

void MemoryResourceManager::releaseMemoryResource(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantMemoryStats_.find(tenantId);
    if (it == tenantMemoryStats_.end()) {
        return;
    }

    allocatedTotalMB_ -= it->second.quotaMB;
    tenantMemoryStats_.erase(it);

    std::cout << "Released memory resources for tenant: " << tenantId << std::endl;
}

} // namespace yao