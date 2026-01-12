#pragma once

#include <string>
#include <stdexcept>

namespace yao {

/**
 * @brief 异常类
 */
class YaoException : public std::runtime_error {
public:
    explicit YaoException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief 租户相关异常
 */
class TenantException : public YaoException {
public:
    explicit TenantException(const std::string& message) : YaoException(message) {}
};

/**
 * @brief 资源相关异常
 */
class ResourceException : public YaoException {
public:
    explicit ResourceException(const std::string& message) : YaoException(message) {}
};

} // namespace yao