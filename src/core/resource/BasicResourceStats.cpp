#include "ResourceStats.h"
#include <atomic>
#include <iostream>

namespace yao {

/**
 * @brief 资源统计实现类
 * 提供基本的资源使用统计功能
 */
class BasicResourceStats : public ResourceStats {
public:
    BasicResourceStats() = default;
    ~BasicResourceStats() override = default;

    double getCpuUsage() const override {
        return cpuUsage_.load();
    }

    size_t getMemoryUsage() const override {
        return memoryUsage_.load();
    }

    size_t getDiskUsage() const override {
        return diskUsage_.load();
    }

    void reset() override {
        cpuUsage_.store(0.0);
        memoryUsage_.store(0);
        diskUsage_.store(0);
    }

    /**
     * @brief 更新CPU使用率
     * @param usage CPU使用率增量
     */
    void updateCpuUsage(double usage) {
        cpuUsage_.fetch_add(usage);
    }

    /**
     * @brief 更新内存使用量
     * @param usage 内存使用量增量
     */
    void updateMemoryUsage(size_t usage) {
        memoryUsage_.fetch_add(usage);
    }

    /**
     * @brief 更新磁盘使用量
     * @param usage 磁盘使用量增量
     */
    void updateDiskUsage(size_t usage) {
        diskUsage_.fetch_add(usage);
    }

private:
    std::atomic<double> cpuUsage_{0.0};
    std::atomic<size_t> memoryUsage_{0};
    std::atomic<size_t> diskUsage_{0};
};

} // namespace yao