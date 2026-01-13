#include "core/tenant/TenantContext.h"

namespace yao {

TenantContext::TenantContext(std::string tenantId, int cpuQuota, size_t memoryQuota, size_t diskQuota)
    : m_tenantId(std::move(tenantId))
    , m_cpuQuota(cpuQuota)
    , m_memoryQuota(memoryQuota)
    , m_diskQuota(diskQuota) {
}

const std::string& TenantContext::getTenantId() const {
    return m_tenantId;
}

int TenantContext::getCpuQuota() const {
    return m_cpuQuota;
}

size_t TenantContext::getMemoryQuota() const {
    return m_memoryQuota;
}

size_t TenantContext::getDiskQuota() const {
    return m_diskQuota;
}

void TenantContext::setCpuQuota(int quota) {
    m_cpuQuota = quota;
}

void TenantContext::setMemoryQuota(size_t quota) {
    m_memoryQuota = quota;
}

void TenantContext::setDiskQuota(size_t quota) {
    m_diskQuota = quota;
}

} // namespace yao