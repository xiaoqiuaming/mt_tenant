#pragma once

#include <memory>
#include <string>
#include <atomic>

namespace yao {

// 前向声明
class RequestContext;
class ConnectionManager;
class ThreadPoolManager;
class Task;

/**
 * @brief SQL服务器接口
 * 定义SQL服务器的基本操作
 */
class SqlServer {
public:
    virtual ~SqlServer() = default;

    /**
     * @brief 处理请求
     * @param context 请求上下文
     * @return 是否处理成功
     */
    virtual bool handleRequest(const RequestContext& context) = 0;

    /**
     * @brief 初始化服务器
     * @return 是否初始化成功
     */
    virtual bool initialize() = 0;

    /**
     * @brief 启动服务器
     * @return 是否启动成功
     */
    virtual bool start() = 0;

    /**
     * @brief 停止服务器
     */
    virtual void stop() = 0;
};

/**
 * @brief YaoBase SQL服务器实现
 * 实现租户级CPU隔离的SQL服务器
 */
class YaoSqlServer : public SqlServer {
public:
    YaoSqlServer();
    ~YaoSqlServer() override;

    bool handleRequest(const RequestContext& context) override;
    bool initialize() override;
    bool start() override;
    void stop() override;

private:
    /**
     * @brief 执行SQL查询
     * @param sql SQL语句
     * @param context 请求上下文
     * @return 是否执行成功
     */
    bool executeSql(const std::string& sql, const RequestContext& context);

    std::unique_ptr<ConnectionManager> connectionManager_;
    std::atomic<bool> running_;
};

} // namespace yao