#pragma once

#include <iostream>
#include <memory>

namespace yao {

// 前向声明
class RequestContext;

/**
 * @brief 管理服务器接口
 * 定义管理服务器的基本操作
 */
class AdminServer {
public:
    virtual ~AdminServer() = default;

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
 * @brief 管理服务器实现
 */
class YaoAdminServer : public AdminServer {
public:
    bool handleRequest(const RequestContext& context) override;
    bool initialize() override;
    bool start() override;
    void stop() override;
};

} // namespace yao