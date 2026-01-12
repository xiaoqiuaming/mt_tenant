#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <unordered_map>

namespace yao {

/**
 * @brief CPU监控器
 * 负责实时监控和统计CPU使用情况
 */
class CpuMonitor {
public:
    /**
     * @brief 获取单例实例
     * @return CpuMonitor实例
     */
    static CpuMonitor& getInstance();

    /**
     * @brief 启动监控
     * @param intervalMs 监控间隔（毫秒）
     * @return 是否启动成功
     */
    bool startMonitoring(int intervalMs = 1000);

    /**
     * @brief 停止监控
     */
    void stopMonitoring();

    /**
     * @brief 注册租户监控
     * @param tenantId 租户ID
     */
    void registerTenant(const std::string& tenantId);

    /**
     * @brief 注销租户监控
     * @param tenantId 租户ID
     */
    void unregisterTenant(const std::string& tenantId);

private:
    CpuMonitor() = default;
    ~CpuMonitor() { stopMonitoring(); }

    CpuMonitor(const CpuMonitor&) = delete;
    CpuMonitor& operator=(const CpuMonitor&) = delete;

    /**
     * @brief 监控循环
     */
    void monitorLoop();

    std::thread monitorThread_;
    std::atomic<bool> running_ = false;
    int intervalMs_ = 1000;
    std::unordered_map<std::string, double> tenantUsages_;
};

} // namespace yao