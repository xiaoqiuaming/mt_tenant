#pragma once

#include <string>
#include <memory>

namespace yao {

/**
 * @brief 租户上下文类
 * 包含租户的基本信息和资源配额
 */
class TenantContext {
public:
    /**
     * @brief 构造函数
     * @param tenantId 租户ID
     * @param cpuQuota CPU配额（核心数）
     * @param memoryQuota 内存配额（字节）
     * @param diskQuota 磁盘配额（字节）
     */
    TenantContext(std::string tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota);

    /**
     * @brief 获取租户ID
     * @return 租户ID
     */
    const std::string& getTenantId() const;

    /**
     * @brief 获取CPU配额
     * @return CPU配额
     */
    int getCpuQuota() const;

    /**
     * @brief 获取内存配额
     * @return 内存配额
     */
    size_t getMemoryQuota() const;

    /**
     * @brief 获取磁盘配额
     * @return 磁盘配额
     */
    size_t getDiskQuota() const;

    /**
     * @brief 设置CPU配额
     * @param quota CPU配额
     */
    void setCpuQuota(int quota);

    /**
     * @brief 设置内存配额
     * @param quota 内存配额
     */
    void setMemoryQuota(size_t quota);

    /**
     * @brief 设置磁盘配额
     * @param quota 磁盘配额
     */
    void setDiskQuota(size_t quota);

private:
    std::string m_tenantId;      ///< 租户ID
    int m_cpuQuota;              ///< CPU配额
    size_t m_memoryQuota;        ///< 内存配额
    size_t m_diskQuota;          ///< 磁盘配额
};

} // namespace yao