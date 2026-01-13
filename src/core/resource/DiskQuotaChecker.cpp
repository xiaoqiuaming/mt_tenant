#include "core/resource/DiskQuotaChecker.h"
#include "core/tenant/TenantContext.h"
#include "core/resource/DiskResourceManager.h"
#include <iostream>

namespace yao {

DiskQuotaChecker& DiskQuotaChecker::getInstance() {
    static DiskQuotaChecker instance;
    return instance;
}

bool DiskQuotaChecker::checkQuota(const std::shared_ptr<TenantContext>& tenant, double requestedGB) {
    if (!tenant) {
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();
    auto& diskManager = DiskResourceManager::getInstance();

    // 获取当前磁盘使用率
    double currentUsage = diskManager.getTenantDiskUsage(tenantId);
    if (currentUsage < 0) {
        // 未分配资源
        return false;
    }

    // 检查配额
    if (!diskManager.checkDiskQuota(tenantId, requestedGB)) {
        std::cerr << "Disk quota exceeded for tenant: " << tenantId
                  << " (current: " << currentUsage * 100 << "%, requested: " << requestedGB << " GB)" << std::endl;

        if (quotaExceededCallback_) {
            double quotaGB = 0.0;
            // 获取配额（简化）
            quotaExceededCallback_(tenantId, currentUsage, quotaGB);
        }
        return false;
    }

    // 检查软限制
    if (currentUsage >= softLimitThreshold_) {
        std::cout << "Warning: Disk usage near soft limit for tenant: " << tenantId
                  << " (" << currentUsage * 100 << "%)" << std::endl;
    }

    // 检查硬限制
    if (currentUsage >= hardLimitThreshold_) {
        std::cerr << "Hard disk limit reached for tenant: " << tenantId
                  << " (" << currentUsage * 100 << "%)" << std::endl;
        return false;
    }

    return true;
}

} // namespace yao