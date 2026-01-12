#pragma once

#include <string>

namespace yao {

/**
 * @brief CPU配额检查器
 */
class CpuQuotaChecker {
public:
    /**
     * @brief 检查CPU配额
     * @param tenantId 租户ID
     * @return 是否允许执行
     */
    bool checkQuota(const std::string& tenantId) const;

    /**
     * @brief 更新配额使用
     * @param tenantId 租户ID
     * @param usage CPU使用量
     */
    void updateUsage(const std::string& tenantId, double usage);
};

} // namespace yao