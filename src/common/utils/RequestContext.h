#pragma once

#include <memory>
#include <string>

namespace yao {

// 前向声明
class TenantContext;
class ResourceStats;

/**
 * @brief 请求上下文类
 * 包含请求相关的租户信息和资源统计
 */
class RequestContext {
public:
    /**
     * @brief 构造函数
     * @param tenant 租户上下文
     * @param stats 资源统计接口
     */
    RequestContext(std::shared_ptr<TenantContext> tenant, std::unique_ptr<ResourceStats> stats);

    /**
     * @brief 获取租户上下文
     * @return 租户上下文
     */
    const std::shared_ptr<TenantContext>& getTenant() const;

    /**
     * @brief 获取资源统计（只读）
     * @return 资源统计接口
     */
    const std::unique_ptr<ResourceStats>& getStats() const;

    /**
     * @brief 获取资源统计（可修改）
     * @return 资源统计接口
     */
    std::unique_ptr<ResourceStats>& getStats();

private:
    std::shared_ptr<TenantContext> m_tenant;  ///< 租户上下文
    std::unique_ptr<ResourceStats> m_stats;   ///< 资源统计
};

} // namespace yao