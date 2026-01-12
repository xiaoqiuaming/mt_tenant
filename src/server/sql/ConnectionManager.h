#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <atomic>

namespace yao {

// 前向声明
class RequestContext;
class Task;

/**
 * @brief SQL任务类
 * 封装SQL请求执行任务
 */
class SqlTask : public Task {
public:
    SqlTask(std::string sql, std::shared_ptr<RequestContext> context);
    ~SqlTask() override = default;

    void execute() override;
    bool isValid() const override;

private:
    std::string sql_;
    std::shared_ptr<RequestContext> context_;
    bool executed_;
};

/**
 * @brief CPU配额检查器
 */
class CpuQuotaChecker {
public:
    /**
     * @brief 检查CPU配额
     * @param tenantId 租户ID
     * @return 是否允许执行
     */
    bool checkQuota(const std::string& tenantId) const;

    /**
     * @brief 更新配额使用
     * @param tenantId 租户ID
     * @param usage CPU使用量
     */
    void updateUsage(const std::string& tenantId, double usage);
};

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

/**
 * @brief 连接管理器
 */
class ConnectionManager {
public:
    ConnectionManager();
    ~ConnectionManager();

    /**
     * @brief 处理新连接
     * @param user 用户名
     * @param password 密码
     * @return 连接上下文，失败返回nullptr
     */
    std::shared_ptr<RequestContext> handleConnection(const std::string& user, const std::string& password);

private:
    TenantAuthenticator authenticator_;
    CpuQuotaChecker quotaChecker_;
};

} // namespace yao