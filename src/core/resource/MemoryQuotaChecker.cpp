#include "core/resource/MemoryQuotaChecker.h"
#include "core/tenant/TenantContext.h"
#include "core/resource/MemoryResourceManager.h"
#include <iostream>

namespace yao {

MemoryQuotaChecker& MemoryQuotaChecker::getInstance() {
    static MemoryQuotaChecker instance;
    return instance;
}

bool MemoryQuotaChecker::checkQuota(const std::shared_ptr<TenantContext>& tenant, double requestedMB) {
    if (!tenant) {
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();
    auto& memoryManager = MemoryResourceManager::getInstance();

    // 获取当前内存使用率
    double currentUsage = memoryManager.getTenantMemoryUsage(tenantId);
    if (currentUsage < 0) {
        // 未分配资源
        return false;
    }

    // 检查配额
    if (!memoryManager.checkMemoryQuota(tenantId, requestedMB)) {
        std::cerr << "Memory quota exceeded for tenant: " << tenantId
                  << " (current: " << currentUsage * 100 << "%, requested: " << requestedMB << " MB)" << std::endl;

        if (quotaExceededCallback_) {
            double quotaMB = 0.0;
            {
                // 获取配额（简化，这里假设从manager获取）
                // 实际应该在MemoryResourceManager中添加获取配额的方法
            }
            quotaExceededCallback_(tenantId, currentUsage, quotaMB);
        }
        return false;
    }

    // 检查软限制
    if (currentUsage >= softLimitThreshold_) {
        std::cout << "Warning: Memory usage near soft limit for tenant: " << tenantId
                  << " (" << currentUsage * 100 << "%)" << std::endl;
    }

    // 检查硬限制
    if (currentUsage >= hardLimitThreshold_) {
        std::cerr << "Hard memory limit reached for tenant: " << tenantId
                  << " (" << currentUsage * 100 << "%)" << std::endl;
        return false;
    }

    return true;
}

} // namespace yao