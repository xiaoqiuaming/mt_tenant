#pragma once

#include "ResourceStats.h"
#include "TenantContext.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>

namespace yao {

/**
 * @brief CPU资源管理器
 * 负责CPU资源的分配、监控和cgroup集成
 */
class CpuResourceManager {
public:
    /**
     * @brief 获取单例实例
     * @return CpuResourceManager实例
     */
    static CpuResourceManager& getInstance();

    /**
     * @brief 初始化cgroup支持
     * @param enableCgroup 是否启用cgroup
     * @return 是否初始化成功
     */
    bool initializeCgroup(bool enableCgroup);

    /**
     * @brief 分配CPU资源给租户
     * @param tenant 租户上下文
     * @return 是否分配成功
     */
    bool allocateCpuResource(const std::shared_ptr<TenantContext>& tenant);

    /**
     * @brief 释放租户的CPU资源
     * @param tenantId 租户ID
     * @return 是否释放成功
     */
    bool releaseCpuResource(const std::string& tenantId);

    /**
     * @brief 获取租户CPU使用统计
     * @param tenantId 租户ID
     * @return CPU使用率，如果租户不存在返回-1
     */
    double getTenantCpuUsage(const std::string& tenantId) const;

    /**
     * @brief 更新CPU使用统计
     * @param tenantId 租户ID
     * @param usage CPU使用率
     */
    void updateCpuUsage(const std::string& tenantId, double usage);

private:
    CpuResourceManager() = default;
    ~CpuResourceManager() = default;
    CpuResourceManager(const CpuResourceManager&) = delete;
    CpuResourceManager& operator=(const CpuResourceManager&) = delete;

    /**
     * @brief 设置cgroup CPU配额
     * @param tenantId 租户ID
     * @param quota CPU配额
     * @return 是否设置成功
     */
    bool setCgroupCpuQuota(const std::string& tenantId, int quota);

    mutable std::mutex m_mutex;  ///< 互斥锁
    bool m_cgroupEnabled = false;  ///< 是否启用cgroup
    std::unordered_map<std::string, double> m_cpuUsage;  ///< 租户CPU使用率映射
};

} // namespace yao