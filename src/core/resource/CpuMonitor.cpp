#include "core/resource/CpuMonitor.h"
#include "core/resource/CpuResourceManager.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>  // for rand()

namespace yao {

CpuMonitor& CpuMonitor::getInstance() {
    static CpuMonitor instance;
    return instance;
}

bool CpuMonitor::startMonitoring(int intervalMs) {
    if (running_) {
        return false;
    }
    intervalMs_ = intervalMs;
    running_ = true;
    monitorThread_ = std::thread(&CpuMonitor::monitorLoop, this);
    return true;
}

void CpuMonitor::stopMonitoring() {
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

void CpuMonitor::registerTenant(const std::string& tenantId) {
    tenantUsages_[tenantId] = 0.0;
}

void CpuMonitor::unregisterTenant(const std::string& tenantId) {
    tenantUsages_.erase(tenantId);
}

void CpuMonitor::monitorLoop() {
    while (running_) {
        // 模拟收集CPU使用情况（实际应从系统获取）
        for (auto& pair : tenantUsages_) {
            // 随机使用率用于演示
            double usage = (std::rand() % 100) / 100.0;
            pair.second = usage;
            CpuResourceManager::getInstance().updateCpuUsage(pair.first, usage);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs_));
    }
}

} // namespace yao