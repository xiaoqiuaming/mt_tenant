#include <gtest/gtest.h>
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief TenantManager 单元测试类
 */
class TenantManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前清理可能存在的租户
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("test_tenant1");
        manager.removeTenant("test_tenant2");
        manager.removeTenant("test_tenant3");
    }

    void TearDown() override {
        // 清理测试创建的租户
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("test_tenant1");
        manager.removeTenant("test_tenant2");
        manager.removeTenant("test_tenant3");
    }
};

/**
 * @brief 测试创建租户
 */
TEST_F(TenantManagerTest, CreateTenant) {
    auto& manager = TenantManager::getInstance();
    
    bool result = manager.createTenant("test_tenant1", 2, 
                                      1024 * 1024 * 1024, 
                                      10LL * 1024 * 1024 * 1024);
    EXPECT_TRUE(result);
    
    auto tenant = manager.getTenant("test_tenant1");
    ASSERT_NE(tenant, nullptr);
    EXPECT_EQ(tenant->getTenantId(), "test_tenant1");
    EXPECT_EQ(tenant->getCpuQuota(), 2);
}

/**
 * @brief 测试获取不存在的租户
 */
TEST_F(TenantManagerTest, GetNonExistentTenant) {
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("nonexistent_tenant");
    EXPECT_EQ(tenant, nullptr);
}

/**
 * @brief 测试创建重复租户
 */
TEST_F(TenantManagerTest, CreateDuplicateTenant) {
    auto& manager = TenantManager::getInstance();
    
    bool result1 = manager.createTenant("test_tenant1", 2, 
                                       1024 * 1024 * 1024, 
                                       10LL * 1024 * 1024 * 1024);
    EXPECT_TRUE(result1);
    
    // 尝试创建同名租户应该失败
    bool result2 = manager.createTenant("test_tenant1", 4, 
                                       2LL * 1024 * 1024 * 1024, 
                                       20LL * 1024 * 1024 * 1024);
    EXPECT_FALSE(result2);
    
    // 验证原租户配置未改变
    auto tenant = manager.getTenant("test_tenant1");
    ASSERT_NE(tenant, nullptr);
    EXPECT_EQ(tenant->getCpuQuota(), 2);
}

/**
 * @brief 测试删除租户
 */
TEST_F(TenantManagerTest, RemoveTenant) {
    auto& manager = TenantManager::getInstance();
    
    manager.createTenant("test_tenant1", 2, 
                        1024 * 1024 * 1024, 
                        10LL * 1024 * 1024 * 1024);
    
    bool result = manager.removeTenant("test_tenant1");
    EXPECT_TRUE(result);
    
    auto tenant = manager.getTenant("test_tenant1");
    EXPECT_EQ(tenant, nullptr);
}

/**
 * @brief 测试删除不存在的租户
 */
TEST_F(TenantManagerTest, RemoveNonExistentTenant) {
    auto& manager = TenantManager::getInstance();
    
    bool result = manager.removeTenant("nonexistent_tenant");
    EXPECT_FALSE(result);
}

/**
 * @brief 测试更新租户配额
 */
TEST_F(TenantManagerTest, UpdateTenantQuota) {
    auto& manager = TenantManager::getInstance();
    
    manager.createTenant("test_tenant1", 2, 
                        1024 * 1024 * 1024, 
                        10LL * 1024 * 1024 * 1024);
    
    bool result = manager.updateTenantQuota("test_tenant1", 4, 
                                           2LL * 1024 * 1024 * 1024, 
                                           20LL * 1024 * 1024 * 1024);
    EXPECT_TRUE(result);
    
    auto tenant = manager.getTenant("test_tenant1");
    ASSERT_NE(tenant, nullptr);
    EXPECT_EQ(tenant->getCpuQuota(), 4);
    EXPECT_EQ(tenant->getMemoryQuota(), 2LL * 1024 * 1024 * 1024);
    EXPECT_EQ(tenant->getDiskQuota(), 20LL * 1024 * 1024 * 1024);
}

/**
 * @brief 测试更新不存在租户的配额
 */
TEST_F(TenantManagerTest, UpdateNonExistentTenantQuota) {
    auto& manager = TenantManager::getInstance();
    
    bool result = manager.updateTenantQuota("nonexistent_tenant", 4, 
                                           2LL * 1024 * 1024 * 1024, 
                                           20LL * 1024 * 1024 * 1024);
    EXPECT_FALSE(result);
}

/**
 * @brief 测试多租户管理
 */
TEST_F(TenantManagerTest, MultiTenantManagement) {
    auto& manager = TenantManager::getInstance();
    
    // 创建多个租户
    EXPECT_TRUE(manager.createTenant("test_tenant1", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024));
    EXPECT_TRUE(manager.createTenant("test_tenant2", 4, 2LL * 1024 * 1024 * 1024, 20LL * 1024 * 1024 * 1024));
    EXPECT_TRUE(manager.createTenant("test_tenant3", 1, 512 * 1024 * 1024, 5LL * 1024 * 1024 * 1024));
    
    // 验证每个租户都存在且配置正确
    auto tenant1 = manager.getTenant("test_tenant1");
    auto tenant2 = manager.getTenant("test_tenant2");
    auto tenant3 = manager.getTenant("test_tenant3");
    
    ASSERT_NE(tenant1, nullptr);
    ASSERT_NE(tenant2, nullptr);
    ASSERT_NE(tenant3, nullptr);
    
    EXPECT_EQ(tenant1->getCpuQuota(), 2);
    EXPECT_EQ(tenant2->getCpuQuota(), 4);
    EXPECT_EQ(tenant3->getCpuQuota(), 1);
    
    // 删除一个租户
    EXPECT_TRUE(manager.removeTenant("test_tenant2"));
    EXPECT_EQ(manager.getTenant("test_tenant2"), nullptr);
    
    // 验证其他租户仍然存在
    EXPECT_NE(manager.getTenant("test_tenant1"), nullptr);
    EXPECT_NE(manager.getTenant("test_tenant3"), nullptr);
}

/**
 * @brief 测试租户上下文共享
 */
TEST_F(TenantManagerTest, TenantContextSharing) {
    auto& manager = TenantManager::getInstance();
    
    manager.createTenant("test_tenant1", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    
    // 获取同一租户的多个引用
    auto tenant1 = manager.getTenant("test_tenant1");
    auto tenant2 = manager.getTenant("test_tenant1");
    
    ASSERT_NE(tenant1, nullptr);
    ASSERT_NE(tenant2, nullptr);
    
    // 验证是同一个对象
    EXPECT_EQ(tenant1.get(), tenant2.get());
    
    // 通过一个引用修改配额
    tenant1->setCpuQuota(4);
    
    // 验证另一个引用也能看到修改
    EXPECT_EQ(tenant2->getCpuQuota(), 4);
}
