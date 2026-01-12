#pragma once

#include <string>

namespace yao {

/**
 * @brief 租户认证器
 */
class TenantAuthenticator {
public:
    /**
     * @brief 认证租户
     * @param user 用户名（格式：user@tenant）
     * @param password 密码
     * @return 认证成功的租户ID，失败返回空字符串
     */
    std::string authenticate(const std::string& user, const std::string& password) const;
};

} // namespace yao