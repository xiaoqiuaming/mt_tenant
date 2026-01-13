#include "core/resource/CpuQuotaChecker.h"
#include "core/resource/CpuResourceManager.h"
#include "core/tenant/TenantManager.h"
#include <iostream>

namespace yao {

bool CpuQuotaChecker::checkQuota(const std::string& tenantId) const {
    auto& cpuManager = CpuResourceManager::getInstance();
    double usage = cpuManager.getTenantCpuUsage(tenantId);
    if (usage < 0) {
        return false;  // 租户不存在
    }
    // 假设cpuQuota是最大使用百分比（例如，cpuQuota=50表示50%）
    auto& tenantManager = TenantManager::getInstance();
    auto tenant = tenantManager.getTenant(tenantId);
    if (!tenant) {
        return false;
    }
    int quota = tenant->getCpuQuota();
    return usage < quota;
}

void CpuQuotaChecker::updateUsage(const std::string& tenantId, double usage) {
    auto& cpuManager = CpuResourceManager::getInstance();
    cpuManager.updateCpuUsage(tenantId, usage);
    std::cout << "Updated CPU usage for tenant " << tenantId << ": " << usage << std::endl;
}

} // namespace yao