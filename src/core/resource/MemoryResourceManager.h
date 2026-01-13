#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>

namespace yao {

class TenantContext;

/**
 * @brief 内存资源管理器
 * 负责管理租户的内存资源分配和监控
 */
class MemoryResourceManager {
public:
    static MemoryResourceManager& getInstance();

    // 初始化
    bool initialize(size_t totalMemoryMB = 8192);  // 默认8GB

    // 内存资源分配
    bool allocateMemoryResource(const std::shared_ptr<TenantContext>& tenant);

    // 获取租户内存使用率
    double getTenantMemoryUsage(const std::string& tenantId);

    // 更新内存使用统计
    void updateMemoryUsage(const std::string& tenantId, double usageMB);

    // 检查内存配额
    bool checkMemoryQuota(const std::string& tenantId, double requestedMB);

    // 释放租户内存资源
    void releaseMemoryResource(const std::string& tenantId);

    // 获取总内存限制
    size_t getTotalMemoryLimit() const { return totalMemoryMB_; }

private:
    MemoryResourceManager() = default;
    ~MemoryResourceManager() = default;
    MemoryResourceManager(const MemoryResourceManager&) = delete;
    MemoryResourceManager& operator=(const MemoryResourceManager&) = delete;

    // 租户内存使用统计
    struct MemoryStats {
        double allocatedMB = 0.0;      // 已分配内存
        double usedMB = 0.0;           // 当前使用内存
        double quotaMB = 0.0;          // 内存配额
        std::atomic<double> peakUsage;  // 峰值使用

        MemoryStats() : allocatedMB(0.0), usedMB(0.0), quotaMB(0.0), peakUsage(0.0) {}
        MemoryStats(double quota, double allocated, double used, double peak)
            : allocatedMB(allocated), usedMB(used), quotaMB(quota), peakUsage(peak) {}
        
        // Move constructor
        MemoryStats(MemoryStats&& other) noexcept
            : allocatedMB(other.allocatedMB)
            , usedMB(other.usedMB)
            , quotaMB(other.quotaMB)
            , peakUsage(other.peakUsage.load()) {}
        
        // Move assignment
        MemoryStats& operator=(MemoryStats&& other) noexcept {
            allocatedMB = other.allocatedMB;
            usedMB = other.usedMB;
            quotaMB = other.quotaMB;
            peakUsage.store(other.peakUsage.load());
            return *this;
        }
        
        // Delete copy
        MemoryStats(const MemoryStats&) = delete;
        MemoryStats& operator=(const MemoryStats&) = delete;
    };

    std::unordered_map<std::string, MemoryStats> tenantMemoryStats_;
    mutable std::mutex mutex_;
    size_t totalMemoryMB_ = 0;
    std::atomic<size_t> allocatedTotalMB_ = 0;
};

} // namespace yao