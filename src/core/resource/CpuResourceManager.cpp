#include "core/resource/CpuResourceManager.h"
#include <iostream>  // 临时用于输出，实际应使用日志

namespace yao {

CpuResourceManager& CpuResourceManager::getInstance() {
    static CpuResourceManager instance;
    return instance;
}

bool CpuResourceManager::initializeCgroup(bool enableCgroup) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cgroupEnabled = enableCgroup;
    if (enableCgroup) {
        // TODO: 初始化cgroup目录和权限
        std::cout << "Cgroup initialized" << std::endl;
    }
    return true;
}

bool CpuResourceManager::allocateCpuResource(const std::shared_ptr<TenantContext>& tenant) {
    if (!tenant) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    const std::string& tenantId = tenant->getTenantId();

    if (m_cpuUsage.find(tenantId) != m_cpuUsage.end()) {
        return false;  // 已分配
    }

    m_cpuUsage[tenantId] = 0.0;

    if (m_cgroupEnabled) {
        return setCgroupCpuQuota(tenantId, tenant->getCpuQuota());
    }

    return true;
}

bool CpuResourceManager::releaseCpuResource(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cpuUsage.erase(tenantId);

    if (m_cgroupEnabled) {
        // TODO: 清理cgroup设置
        std::cout << "Released cgroup for tenant: " << tenantId << std::endl;
    }

    return true;
}

double CpuResourceManager::getTenantCpuUsage(const std::string& tenantId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cpuUsage.find(tenantId);
    if (it != m_cpuUsage.end()) {
        return it->second;
    }
    return -1.0;
}

void CpuResourceManager::updateCpuUsage(const std::string& tenantId, double usage) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cpuUsage[tenantId] = usage;
}

bool CpuResourceManager::setCgroupCpuQuota(const std::string& tenantId, int quota) {
    // TODO: 实现cgroup CPU配额设置
    // 例如：写入 /sys/fs/cgroup/cpu/yaobase/tenantId/cpu.shares
    std::cout << "Setting cgroup CPU quota for tenant " << tenantId << " to " << quota << std::endl;
    return true;
}

} // namespace yao