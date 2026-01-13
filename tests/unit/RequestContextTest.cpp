#include <gtest/gtest.h>
#include "common/utils/RequestContext.h"
#include "core/tenant/TenantContext.h"
#include "core/resource/BasicResourceStats.h"

using namespace yao;

/**
 * @brief RequestContext 单元测试类
 */
class RequestContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 准备测试数据
    }

    void TearDown() override {
        // 清理
    }
};

/**
 * @brief 测试RequestContext构造
 */
TEST_F(RequestContextTest, ConstructorInitializesCorrectly) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats = std::make_unique<BasicResourceStats>();
    
    RequestContext context(tenant, std::move(stats));
    
    EXPECT_NE(context.getTenant(), nullptr);
    EXPECT_EQ(context.getTenant()->getTenantId(), "test_tenant");
    EXPECT_NE(context.getStats(), nullptr);
}

/**
 * @brief 测试获取租户上下文
 */
TEST_F(RequestContextTest, GetTenant) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats = std::make_unique<BasicResourceStats>();
    
    RequestContext context(tenant, std::move(stats));
    
    auto retrievedTenant = context.getTenant();
    ASSERT_NE(retrievedTenant, nullptr);
    EXPECT_EQ(retrievedTenant->getTenantId(), "test_tenant");
    EXPECT_EQ(retrievedTenant->getCpuQuota(), 2);
}

/**
 * @brief 测试获取资源统计（只读）
 */
TEST_F(RequestContextTest, GetStatsConst) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats = std::make_unique<BasicResourceStats>();
    
    RequestContext context(tenant, std::move(stats));
    
    const auto& retrievedStats = context.getStats();
    EXPECT_NE(retrievedStats, nullptr);
}

/**
 * @brief 测试获取资源统计（可修改）
 */
TEST_F(RequestContextTest, GetStatsNonConst) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats = std::make_unique<BasicResourceStats>();
    
    RequestContext context(tenant, std::move(stats));
    
    auto& retrievedStats = context.getStats();
    EXPECT_NE(retrievedStats, nullptr);
    
    // 可以通过非const引用修改stats
    retrievedStats->reset();
}

/**
 * @brief 测试租户上下文共享
 */
TEST_F(RequestContextTest, TenantContextSharing) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats1 = std::make_unique<BasicResourceStats>();
    auto stats2 = std::make_unique<BasicResourceStats>();
    
    // 两个context共享同一个tenant
    RequestContext context1(tenant, std::move(stats1));
    RequestContext context2(tenant, std::move(stats2));
    
    // 验证共享同一个租户对象
    EXPECT_EQ(context1.getTenant().get(), context2.getTenant().get());
    
    // 通过一个context修改tenant
    context1.getTenant()->setCpuQuota(4);
    
    // 验证另一个context也能看到修改
    EXPECT_EQ(context2.getTenant()->getCpuQuota(), 4);
}

/**
 * @brief 测试不同的资源统计对象
 */
TEST_F(RequestContextTest, DifferentResourceStats) {
    auto tenant = std::make_shared<TenantContext>("test_tenant", 2, 
                                                   1024 * 1024 * 1024, 
                                                   10LL * 1024 * 1024 * 1024);
    auto stats1 = std::make_unique<BasicResourceStats>();
    auto stats2 = std::make_unique<BasicResourceStats>();
    
    RequestContext context1(tenant, std::move(stats1));
    RequestContext context2(tenant, std::move(stats2));
    
    // 每个context有自己的stats对象
    EXPECT_NE(context1.getStats().get(), context2.getStats().get());
}
