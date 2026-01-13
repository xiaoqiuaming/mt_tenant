#include "core/resource/TenantAuthenticator.h"
#include "core/tenant/TenantManager.h"

namespace yao {

std::string TenantAuthenticator::authenticate(const std::string& user, const std::string& password) const {
    // 解析用户名格式：user@tenant
    size_t atPos = user.find('@');
    if (atPos == std::string::npos) {
        return "";  // 无效格式
    }

    std::string username = user.substr(0, atPos);
    std::string tenantId = user.substr(atPos + 1);

    // TODO: 实际的认证逻辑（例如，验证密码）
    // 这里简化为检查租户是否存在
    auto& tenantManager = TenantManager::getInstance();
    auto tenant = tenantManager.getTenant(tenantId);
    if (!tenant) {
        return "";  // 租户不存在
    }

    return tenantId;
}

} // namespace yao