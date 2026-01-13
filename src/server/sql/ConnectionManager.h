#pragma once

#include "core/resource/LockFreeQueue.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <atomic>

namespace yao {

// 前向声明
class RequestContext;
class CpuQuotaChecker;
class TenantAuthenticator;

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
    std::unique_ptr<TenantAuthenticator> authenticator_;
    std::unique_ptr<CpuQuotaChecker> quotaChecker_;
};

} // namespace yao