#pragma once

#include "TenantContext.h"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace yao {

/**
 * @brief 租户管理器类
 * 负责租户的创建、查询和管理
 */
class TenantManager {
public:
    /**
     * @brief 获取单例实例
     * @return TenantManager实例
     */
    static TenantManager& getInstance();

    /**
     * @brief 创建租户
     * @param tenantId 租户ID
     * @param cpuQuota CPU配额
     * @param memoryQuota 内存配额
     * @param diskQuota 磁盘配额
     * @return 是否创建成功
     */
    bool createTenant(const std::string& tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota);

    /**
     * @brief 获取租户上下文
     * @param tenantId 租户ID
     * @return 租户上下文指针，如果不存在返回nullptr
     */
    std::shared_ptr<TenantContext> getTenant(const std::string& tenantId) const;

    /**
     * @brief 删除租户
     * @param tenantId 租户ID
     * @return 是否删除成功
     */
    bool removeTenant(const std::string& tenantId);

    /**
     * @brief 更新租户配额
     * @param tenantId 租户ID
     * @param cpuQuota CPU配额
     * @param memoryQuota 内存配额
     * @param diskQuota 磁盘配额
     * @return 是否更新成功
     */
    bool updateTenantQuota(const std::string& tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota);

private:
    TenantManager() = default;
    ~TenantManager() = default;
    TenantManager(const TenantManager&) = delete;
    TenantManager& operator=(const TenantManager&) = delete;

    mutable std::mutex m_mutex;  ///< 互斥锁
    std::unordered_map<std::string, std::shared_ptr<TenantContext>> m_tenants;  ///< 租户映射
};

} // namespace yao