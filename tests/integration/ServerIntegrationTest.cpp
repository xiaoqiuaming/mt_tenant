#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include "server/sql/SqlServer.h"
#include "server/data/DataServer.h"
#include "server/trans/TransServer.h"
#include "server/admin/AdminServer.h"
#include "core/tenant/TenantManager.h"
#include "core/resource/BasicResourceStats.h"
#include "common/utils/RequestContext.h"

using namespace yao;

/**
 * @brief 服务器集成测试类
 */
class ServerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试租户
        auto& tenantManager = TenantManager::getInstance();
        tenantManager.createTenant("integration_tenant1", 2, 
                                  2LL * 1024 * 1024 * 1024, 
                                  20LL * 1024 * 1024 * 1024);
        tenantManager.createTenant("integration_tenant2", 1, 
                                  1024 * 1024 * 1024, 
                                  10LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        // 清理测试租户
        auto& tenantManager = TenantManager::getInstance();
        tenantManager.removeTenant("integration_tenant1");
        tenantManager.removeTenant("integration_tenant2");
    }
};

/**
 * @brief 测试SqlServer初始化和启动
 */
TEST_F(ServerIntegrationTest, SqlServerInitializeAndStart) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    
    sqlServer->stop();
}

/**
 * @brief 测试DataServer初始化和启动
 */
TEST_F(ServerIntegrationTest, DataServerInitializeAndStart) {
    auto dataServer = std::make_unique<YaoDataServer>();
    
    EXPECT_TRUE(dataServer->initialize());
    EXPECT_TRUE(dataServer->start());
    
    dataServer->stop();
}

/**
 * @brief 测试TransServer初始化和启动
 */
TEST_F(ServerIntegrationTest, TransServerInitializeAndStart) {
    auto transServer = std::make_unique<YaoTransServer>();
    
    EXPECT_TRUE(transServer->initialize());
    EXPECT_TRUE(transServer->start());
    
    transServer->stop();
}

/**
 * @brief 测试AdminServer初始化和启动
 */
TEST_F(ServerIntegrationTest, AdminServerInitializeAndStart) {
    auto adminServer = std::make_unique<YaoAdminServer>();
    
    EXPECT_TRUE(adminServer->initialize());
    EXPECT_TRUE(adminServer->start());
    
    adminServer->stop();
}

/**
 * @brief 测试所有服务器同时运行
 */
TEST_F(ServerIntegrationTest, AllServersRunningTogether) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    auto dataServer = std::make_unique<YaoDataServer>();
    auto transServer = std::make_unique<YaoTransServer>();
    auto adminServer = std::make_unique<YaoAdminServer>();
    
    // 初始化所有服务器
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(dataServer->initialize());
    EXPECT_TRUE(transServer->initialize());
    EXPECT_TRUE(adminServer->initialize());
    
    // 启动所有服务器
    EXPECT_TRUE(sqlServer->start());
    EXPECT_TRUE(dataServer->start());
    EXPECT_TRUE(transServer->start());
    EXPECT_TRUE(adminServer->start());
    
    // 让服务器运行一小段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 停止所有服务器
    sqlServer->stop();
    dataServer->stop();
    transServer->stop();
    adminServer->stop();
}

/**
 * @brief 测试SqlServer处理单个租户请求
 */
TEST_F(ServerIntegrationTest, SqlServerHandleSingleTenantRequest) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    auto& tenantManager = TenantManager::getInstance();
    
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    
    auto tenant = tenantManager.getTenant("integration_tenant1");
    ASSERT_NE(tenant, nullptr);
    
    auto stats = std::make_unique<BasicResourceStats>();
    RequestContext context(tenant, std::move(stats));
    
    // 处理请求（假设handleRequest不会失败）
    sqlServer->handleRequest(context);
    
    sqlServer->stop();
}

/**
 * @brief 测试SqlServer处理多个租户请求
 */
TEST_F(ServerIntegrationTest, SqlServerHandleMultipleTenantRequests) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    auto& tenantManager = TenantManager::getInstance();
    
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    
    // 租户1的请求
    auto tenant1 = tenantManager.getTenant("integration_tenant1");
    ASSERT_NE(tenant1, nullptr);
    auto stats1 = std::make_unique<BasicResourceStats>();
    RequestContext context1(tenant1, std::move(stats1));
    sqlServer->handleRequest(context1);
    
    // 租户2的请求
    auto tenant2 = tenantManager.getTenant("integration_tenant2");
    ASSERT_NE(tenant2, nullptr);
    auto stats2 = std::make_unique<BasicResourceStats>();
    RequestContext context2(tenant2, std::move(stats2));
    sqlServer->handleRequest(context2);
    
    sqlServer->stop();
}

/**
 * @brief 测试SqlServer处理连续请求
 */
TEST_F(ServerIntegrationTest, SqlServerHandleSequentialRequests) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    auto& tenantManager = TenantManager::getInstance();
    
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    
    auto tenant = tenantManager.getTenant("integration_tenant1");
    ASSERT_NE(tenant, nullptr);
    
    // 处理多个连续请求
    for (int i = 0; i < 5; i++) {
        auto stats = std::make_unique<BasicResourceStats>();
        RequestContext context(tenant, std::move(stats));
        sqlServer->handleRequest(context);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    sqlServer->stop();
}

/**
 * @brief 测试服务器重启
 */
TEST_F(ServerIntegrationTest, ServerRestart) {
    auto sqlServer = std::make_unique<YaoSqlServer>();
    
    // 第一次启动
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    sqlServer->stop();
    
    // 重新启动
    EXPECT_TRUE(sqlServer->initialize());
    EXPECT_TRUE(sqlServer->start());
    sqlServer->stop();
}
