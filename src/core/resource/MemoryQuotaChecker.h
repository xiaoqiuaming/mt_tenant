#pragma once

#include <string>
#include <functional>

namespace yao {

class TenantContext;

/**
 * @brief 内存配额检查器
 * 检查租户的内存使用是否超过配额
 */
class MemoryQuotaChecker {
public:
    static MemoryQuotaChecker& getInstance();

    // 检查内存配额
    bool checkQuota(const std::shared_ptr<TenantContext>& tenant, double requestedMB);

    // 设置软限制阈值 (默认70%)
    void setSoftLimitThreshold(double threshold) { softLimitThreshold_ = threshold; }

    // 设置硬限制阈值 (默认90%)
    void setHardLimitThreshold(double threshold) { hardLimitThreshold_ = threshold; }

    // 设置配额超限回调
    void setQuotaExceededCallback(std::function<void(const std::string&, double, double)> callback) {
        quotaExceededCallback_ = callback;
    }

private:
    MemoryQuotaChecker() = default;
    ~MemoryQuotaChecker() = default;
    MemoryQuotaChecker(const MemoryQuotaChecker&) = delete;
    MemoryQuotaChecker& operator=(const MemoryQuotaChecker&) = delete;

    double softLimitThreshold_ = 0.7;  // 70%
    double hardLimitThreshold_ = 0.9;  // 90%

    std::function<void(const std::string&, double, double)> quotaExceededCallback_;
};

} // namespace yao