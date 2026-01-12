#include "TenantManager.h"
#include "../resource/CpuResourceManager.h"
#include "../resource/MemoryResourceManager.h"
#include "../resource/DiskResourceManager.h"
#include "../resource/ThreadPoolManager.h"
#include "../resource/CpuMonitor.h"
#include <stdexcept>

namespace yao {

TenantManager& TenantManager::getInstance() {
    static TenantManager instance;
    return instance;
}

bool TenantManager::createTenant(const std::string& tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_tenants.find(tenantId) != m_tenants.end()) {
        return false;  // 租户已存在
    }
    auto tenant = std::make_shared<TenantContext>(tenantId, cpuQuota, memoryQuota, diskQuota);
    m_tenants[tenantId] = tenant;

    // 分配CPU资源
    if (!CpuResourceManager::getInstance().allocateCpuResource(tenant)) {
        m_tenants.erase(tenantId);  // 回滚
        return false;
    }

    // 分配内存资源
    if (!MemoryResourceManager::getInstance().allocateMemoryResource(tenant)) {
        CpuResourceManager::getInstance().releaseCpuResource(tenantId);
        m_tenants.erase(tenantId);  // 回滚
        return false;
    }

    // 分配磁盘资源
    if (!DiskResourceManager::getInstance().allocateDiskResource(tenant)) {
        CpuResourceManager::getInstance().releaseCpuResource(tenantId);
        MemoryResourceManager::getInstance().releaseMemoryResource(tenantId);
        m_tenants.erase(tenantId);  // 回滚
        return false;
    }

    // 注册监控
    CpuMonitor::getInstance().registerTenant(tenantId);

    // 创建租户线程组（假设每核心10个线程）
    size_t threadCount = static_cast<size_t>(cpuQuota) * 10;
    if (!ThreadPoolManager::getInstance().createTenantThreadGroup(tenantId, threadCount)) {
        CpuResourceManager::getInstance().releaseCpuResource(tenantId);
        MemoryResourceManager::getInstance().releaseMemoryResource(tenantId);
        DiskResourceManager::getInstance().releaseDiskResource(tenantId);
        CpuMonitor::getInstance().unregisterTenant(tenantId);
        m_tenants.erase(tenantId);  // 回滚
        return false;
    }

    return true;
}

std::shared_ptr<TenantContext> TenantManager::getTenant(const std::string& tenantId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tenants.find(tenantId);
    if (it != m_tenants.end()) {
        return it->second;
    }
    return nullptr;
}

bool TenantManager::removeTenant(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tenants.find(tenantId);
    if (it == m_tenants.end()) {
        return false;
    }

    // 释放资源
    CpuResourceManager::getInstance().releaseCpuResource(tenantId);
    MemoryResourceManager::getInstance().releaseMemoryResource(tenantId);
    DiskResourceManager::getInstance().releaseDiskResource(tenantId);
    CpuMonitor::getInstance().unregisterTenant(tenantId);
    ThreadPoolManager::getInstance().removeTenantThreadGroup(tenantId);

    return m_tenants.erase(tenantId) > 0;
}

bool TenantManager::updateTenantQuota(const std::string& tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_tenants.find(tenantId);
    if (it == m_tenants.end()) {
        return false;
    }
    it->second->setCpuQuota(cpuQuota);
    it->second->setMemoryQuota(memoryQuota);
    it->second->setDiskQuota(diskQuota);

    // 调整线程组大小
    size_t threadCount = static_cast<size_t>(cpuQuota) * 10;
    ThreadPoolManager::getInstance().resizeTenantThreads(tenantId, threadCount);

    return true;
}

} // namespace yao