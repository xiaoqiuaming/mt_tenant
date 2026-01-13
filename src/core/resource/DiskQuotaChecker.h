#pragma once

#include <string>
#include <functional>
#include <memory>

namespace yao {

class TenantContext;

/**
 * @brief 磁盘配额检查器
 * 检查租户的磁盘使用是否超过配额
 */
class DiskQuotaChecker {
public:
    static DiskQuotaChecker& getInstance();

    // 检查磁盘配额
    bool checkQuota(const std::shared_ptr<TenantContext>& tenant, double requestedGB);

    // 设置软限制阈值 (默认70%)
    void setSoftLimitThreshold(double threshold) { softLimitThreshold_ = threshold; }

    // 设置硬限制阈值 (默认90%)
    void setHardLimitThreshold(double threshold) { hardLimitThreshold_ = threshold; }

    // 设置配额超限回调
    void setQuotaExceededCallback(std::function<void(const std::string&, double, double)> callback) {
        quotaExceededCallback_ = callback;
    }

private:
    DiskQuotaChecker() = default;
    ~DiskQuotaChecker() = default;
    DiskQuotaChecker(const DiskQuotaChecker&) = delete;
    DiskQuotaChecker& operator=(const DiskQuotaChecker&) = delete;

    double softLimitThreshold_ = 0.7;  // 70%
    double hardLimitThreshold_ = 0.9;  // 90%

    std::function<void(const std::string&, double, double)> quotaExceededCallback_;
};

} // namespace yao