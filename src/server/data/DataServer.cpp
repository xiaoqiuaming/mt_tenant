#include "server/data/DataServer.h"
#include "core/resource/DiskResourceManager.h"
#include "core/resource/DiskQuotaChecker.h"
#include "common/utils/RequestContext.h"
#include "core/tenant/TenantContext.h"
#include <iostream>

namespace yao {

bool YaoDataServer::handleRequest(const RequestContext& context) {
    auto tenant = context.getTenant();
    if (!tenant) {
        std::cout << "No tenant context" << std::endl;
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();

    // 检查磁盘资源分配
    auto& diskManager = DiskResourceManager::getInstance();
    double diskUsage = diskManager.getTenantDiskUsage(tenantId);
    if (diskUsage < 0) {
        // 首次请求，分配磁盘资源
        if (!diskManager.allocateDiskResource(tenant)) {
            std::cerr << "Failed to allocate disk resource for tenant: " << tenantId << std::endl;
            return false;
        }
    }

    // 检查磁盘配额
    auto& diskChecker = DiskQuotaChecker::getInstance();
    double requestedDiskGB = 1.0;  // 示例：请求1GB磁盘
    if (!diskChecker.checkQuota(tenant, requestedDiskGB)) {
        std::cerr << "Disk quota check failed for tenant: " << tenantId << std::endl;
        return false;
    }

    // 处理数据请求
    std::cout << "Handling data request for tenant: " << tenantId << std::endl;

    // 更新磁盘使用统计（模拟）
    double currentDiskUsage = diskManager.getTenantDiskUsage(tenantId);
    if (currentDiskUsage >= 0) {
        diskManager.updateDiskUsage(tenantId, currentDiskUsage + requestedDiskGB);
    }

    return true;
}

bool YaoDataServer::initialize() {
    // 初始化磁盘资源管理器
    auto& diskManager = DiskResourceManager::getInstance();
    size_t totalDiskGB = 100;  // 默认100GB，可从配置读取
    if (!diskManager.initialize(totalDiskGB)) {
        std::cerr << "Failed to initialize DiskResourceManager" << std::endl;
        return false;
    }

    std::cout << "YaoDataServer initialized" << std::endl;
    return true;
}

bool YaoDataServer::start() {
    std::cout << "YaoDataServer started" << std::endl;
    return true;
}

void YaoDataServer::stop() {
    std::cout << "YaoDataServer stopped" << std::endl;
}

} // namespace yao