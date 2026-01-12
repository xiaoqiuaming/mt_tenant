#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>

namespace yao {

class TenantContext;

/**
 * @brief 磁盘资源管理器
 * 负责管理租户的磁盘资源分配和监控
 */
class DiskResourceManager {
public:
    static DiskResourceManager& getInstance();

    // 初始化
    bool initialize(size_t totalDiskGB = 100);  // 默认100GB

    // 磁盘资源分配
    bool allocateDiskResource(const std::shared_ptr<TenantContext>& tenant);

    // 获取租户磁盘使用率
    double getTenantDiskUsage(const std::string& tenantId);

    // 更新磁盘使用统计
    void updateDiskUsage(const std::string& tenantId, double usageGB);

    // 检查磁盘配额
    bool checkDiskQuota(const std::string& tenantId, double requestedGB);

    // 释放租户磁盘资源
    void releaseDiskResource(const std::string& tenantId);

    // 获取总磁盘限制
    size_t getTotalDiskLimit() const { return totalDiskGB_; }

private:
    DiskResourceManager() = default;
    ~DiskResourceManager() = default;
    DiskResourceManager(const DiskResourceManager&) = delete;
    DiskResourceManager& operator=(const DiskResourceManager&) = delete;

    // 租户磁盘使用统计
    struct DiskStats {
        double allocatedGB = 0.0;      // 已分配磁盘
        double usedGB = 0.0;           // 当前使用磁盘
        double quotaGB = 0.0;          // 磁盘配额
        std::atomic<double> peakUsage = 0.0;  // 峰值使用
    };

    std::unordered_map<std::string, DiskStats> tenantDiskStats_;
    mutable std::mutex mutex_;
    size_t totalDiskGB_ = 0;
    std::atomic<size_t> allocatedTotalGB_ = 0;
};

} // namespace yao