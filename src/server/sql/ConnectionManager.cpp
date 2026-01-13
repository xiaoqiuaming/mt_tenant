#include "server/sql/ConnectionManager.h"
#include "core/tenant/TenantManager.h"
#include "core/resource/TenantAuthenticator.h"
#include "core/resource/CpuQuotaChecker.h"
#include "common/utils/RequestContext.h"
#include "core/resource/LockFreeQueue.h"
#include "core/resource/BasicResourceStats.h"
#include <iostream>

namespace yao {

SqlTask::SqlTask(std::string sql, std::shared_ptr<RequestContext> context)
    : sql_(std::move(sql)), context_(std::move(context)), executed_(false) {
}

void SqlTask::execute() {
    if (executed_) return;

    executed_ = true;

    // 模拟SQL执行
    std::cout << "Executing SQL: " << sql_ << std::endl;

    // TODO: 实际的SQL解析和执行逻辑
    // 这里应该调用SQL引擎执行查询

    // 更新资源统计
    if (context_ && context_->getStats()) {
        // 尝试转换为BasicResourceStats进行更新
        auto* basicStats = dynamic_cast<BasicResourceStats*>(context_->getStats().get());
        if (basicStats) {
            // 模拟CPU使用
            basicStats->updateCpuUsage(0.05);  // 5% CPU使用
        }
    }
}

bool SqlTask::isValid() const {
    return !sql_.empty() && context_ != nullptr;
}

ConnectionManager::ConnectionManager() 
    : authenticator_(std::make_unique<TenantAuthenticator>())
    , quotaChecker_(std::make_unique<CpuQuotaChecker>()) {
}

ConnectionManager::~ConnectionManager() = default;

std::shared_ptr<RequestContext> ConnectionManager::handleConnection(const std::string& user, const std::string& password) {
    // 认证租户
    std::string tenantId = authenticator_->authenticate(user, password);
    if (tenantId.empty()) {
        std::cerr << "Authentication failed for user: " << user << std::endl;
        return nullptr;
    }

    // 检查CPU配额
    if (!quotaChecker_->checkQuota(tenantId)) {
        std::cerr << "CPU quota exceeded for tenant: " << tenantId << std::endl;
        return nullptr;
    }

    // 获取租户上下文
    auto& tenantManager = TenantManager::getInstance();
    auto tenant = tenantManager.getTenant(tenantId);
    if (!tenant) {
        std::cerr << "Tenant not found: " << tenantId << std::endl;
        return nullptr;
    }

    // 创建请求上下文
    auto stats = std::make_unique<BasicResourceStats>();
    return std::make_shared<RequestContext>(tenant, std::move(stats));
}

} // namespace yao