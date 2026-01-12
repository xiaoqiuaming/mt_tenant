#pragma once

#include "ResourceStats.h"
#include <atomic>

namespace yao {

/**
 * @brief 资源统计实现类
 * 提供基本的资源使用统计功能
 */
class BasicResourceStats : public ResourceStats {
public:
    BasicResourceStats();
    ~BasicResourceStats() override;

    double getCpuUsage() const override;
    size_t getMemoryUsage() const override;
    size_t getDiskUsage() const override;
    void reset() override;

    /**
     * @brief 更新CPU使用率
     * @param usage CPU使用率增量
     */
    void updateCpuUsage(double usage);

    /**
     * @brief 更新内存使用量
     * @param usage 内存使用量增量
     */
    void updateMemoryUsage(size_t usage);

    /**
     * @brief 更新磁盘使用量
     * @param usage 磁盘使用量增量
     */
    void updateDiskUsage(size_t usage);

private:
    std::atomic<double> cpuUsage_;
    std::atomic<size_t> memoryUsage_;
    std::atomic<size_t> diskUsage_;
};

} // namespace yao