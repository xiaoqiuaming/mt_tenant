#include "DiskResourceManager.h"
#include "TenantContext.h"
#include <iostream>
#include <algorithm>

namespace yao {

DiskResourceManager& DiskResourceManager::getInstance() {
    static DiskResourceManager instance;
    return instance;
}

bool DiskResourceManager::initialize(size_t totalDiskGB) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalDiskGB_ = totalDiskGB;
    allocatedTotalGB_ = 0;
    tenantDiskStats_.clear();
    std::cout << "DiskResourceManager initialized with " << totalDiskGB << " GB total disk" << std::endl;
    return true;
}

bool DiskResourceManager::allocateDiskResource(const std::shared_ptr<TenantContext>& tenant) {
    if (!tenant) {
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();
    std::lock_guard<std::mutex> lock(mutex_);

    // 检查是否已分配
    if (tenantDiskStats_.find(tenantId) != tenantDiskStats_.end()) {
        return true;  // 已分配
    }

    // 计算磁盘配额（基于CPU配额的比例）
    double cpuQuota = tenant->getCpuQuota();
    double diskQuotaGB = (cpuQuota / 100.0) * totalDiskGB_ * 0.8;  // 80%比例

    // 检查总分配是否超过限制
    if (allocatedTotalGB_ + diskQuotaGB > totalDiskGB_) {
        std::cerr << "Insufficient disk for tenant: " << tenantId << std::endl;
        return false;
    }

    // 分配磁盘资源
    DiskStats stats;
    stats.quotaGB = diskQuotaGB;
    stats.allocatedGB = 0.0;
    stats.usedGB = 0.0;
    stats.peakUsage = 0.0;

    tenantDiskStats_[tenantId] = stats;
    allocatedTotalGB_ += diskQuotaGB;

    std::cout << "Allocated " << diskQuotaGB << " GB disk for tenant: " << tenantId << std::endl;
    return true;
}

double DiskResourceManager::getTenantDiskUsage(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantDiskStats_.find(tenantId);
    if (it == tenantDiskStats_.end()) {
        return -1.0;  // 未分配
    }
    return it->second.usedGB / it->second.quotaGB;  // 返回使用率
}

void DiskResourceManager::updateDiskUsage(const std::string& tenantId, double usageGB) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantDiskStats_.find(tenantId);
    if (it == tenantDiskStats_.end()) {
        return;
    }

    it->second.usedGB = usageGB;
    it->second.peakUsage = std::max(it->second.peakUsage.load(), usageGB);
}

bool DiskResourceManager::checkDiskQuota(const std::string& tenantId, double requestedGB) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantDiskStats_.find(tenantId);
    if (it == tenantDiskStats_.end()) {
        return false;
    }

    double currentUsage = it->second.usedGB + requestedGB;
    return currentUsage <= it->second.quotaGB;
}

void DiskResourceManager::releaseDiskResource(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tenantDiskStats_.find(tenantId);
    if (it == tenantDiskStats_.end()) {
        return;
    }

    allocatedTotalGB_ -= it->second.quotaGB;
    tenantDiskStats_.erase(it);

    std::cout << "Released disk resources for tenant: " << tenantId << std::endl;
}

} // namespace yao