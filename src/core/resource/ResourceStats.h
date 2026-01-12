#pragma once

#include <string>

namespace yao {

/**
 * @brief 资源统计接口
 * 定义资源使用统计的抽象接口
 */
class ResourceStats {
public:
    virtual ~ResourceStats() = default;

    /**
     * @brief 获取CPU使用率
     * @return CPU使用率（百分比）
     */
    virtual double getCpuUsage() const = 0;

    /**
     * @brief 获取内存使用量
     * @return 内存使用量（字节）
     */
    virtual size_t getMemoryUsage() const = 0;

    /**
     * @brief 获取磁盘使用量
     * @return 磁盘使用量（字节）
     */
    virtual size_t getDiskUsage() const = 0;

    /**
     * @brief 重置统计数据
     */
    virtual void reset() = 0;
};

} // namespace yao