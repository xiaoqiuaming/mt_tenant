#include "common/utils/RequestContext.h"
#include "core/resource/ResourceStats.h"
#include "core/tenant/TenantContext.h"

namespace yao {

RequestContext::RequestContext(std::shared_ptr<TenantContext> tenant, std::unique_ptr<ResourceStats> stats)
    : m_tenant(std::move(tenant))
    , m_stats(std::move(stats)) {
}

const std::shared_ptr<TenantContext>& RequestContext::getTenant() const {
    return m_tenant;
}

const std::unique_ptr<ResourceStats>& RequestContext::getStats() const {
    return m_stats;
}

std::unique_ptr<ResourceStats>& RequestContext::getStats() {
    return m_stats;
}

} // namespace yao