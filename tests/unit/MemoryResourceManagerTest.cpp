#include <gtest/gtest.h>
#include "core/resource/MemoryResourceManager.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief MemoryResourceManager 单元测试类
 */
class MemoryResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("mem_tenant1");
        manager.removeTenant("mem_tenant2");
        manager.createTenant("mem_tenant1", 2, 2LL * 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
        manager.createTenant("mem_tenant2", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("mem_tenant1");
        manager.removeTenant("mem_tenant2");
    }
};

/**
 * @brief 测试初始化内存管理器
 */
TEST_F(MemoryResourceManagerTest, Initialize) {
    auto& memManager = MemoryResourceManager::getInstance();
    
    size_t totalMemoryMB = 8192; // 8GB
    bool result = memManager.initialize(totalMemoryMB);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试分配内存资源
 */
TEST_F(MemoryResourceManagerTest, AllocateMemoryResource) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    ASSERT_NE(tenant, nullptr);
    
    bool result = memManager.allocateMemoryResource(tenant);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试释放内存资源
 */
TEST_F(MemoryResourceManagerTest, ReleaseMemoryResource) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    ASSERT_NE(tenant, nullptr);
    
    memManager.allocateMemoryResource(tenant);
    memManager.releaseMemoryResource("mem_tenant1");
    
    // 验证释放后使用率为0
    double usage = memManager.getTenantMemoryUsage("mem_tenant1");
    EXPECT_EQ(usage, 0.0);
}

/**
 * @brief 测试获取内存使用量
 */
TEST_F(MemoryResourceManagerTest, GetMemoryUsage) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    memManager.allocateMemoryResource(tenant);
    
    // 更新使用情况
    memManager.updateMemoryUsage("mem_tenant1", 512.0); // 512MB
    
    double usage = memManager.getTenantMemoryUsage("mem_tenant1");
    EXPECT_EQ(usage, 512.0);
}

/**
 * @brief 测试检查内存配额
 */
TEST_F(MemoryResourceManagerTest, CheckMemoryQuota) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    memManager.allocateMemoryResource(tenant);
    
    // 租户配额2GB，请求1GB应该成功
    EXPECT_TRUE(memManager.checkMemoryQuota("mem_tenant1", 1024.0));
    
    // 请求3GB应该失败
    EXPECT_FALSE(memManager.checkMemoryQuota("mem_tenant1", 3072.0));
}

/**
 * @brief 测试多次更新使用量
 */
TEST_F(MemoryResourceManagerTest, MultipleUsageUpdates) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    memManager.allocateMemoryResource(tenant);
    
    memManager.updateMemoryUsage("mem_tenant1", 512.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("mem_tenant1"), 512.0);
    
    memManager.updateMemoryUsage("mem_tenant1", 1024.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("mem_tenant1"), 1024.0);
}

/**
 * @brief 测试多租户内存分配
 */
TEST_F(MemoryResourceManagerTest, MultiTenantAllocation) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant1 = tenantManager.getTenant("mem_tenant1");
    auto tenant2 = tenantManager.getTenant("mem_tenant2");
    
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant1));
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant2));
    
    memManager.updateMemoryUsage("mem_tenant1", 1024.0);
    memManager.updateMemoryUsage("mem_tenant2", 512.0);
    
    EXPECT_EQ(memManager.getTenantMemoryUsage("mem_tenant1"), 1024.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("mem_tenant2"), 512.0);
}

/**
 * @brief 测试零使用量
 */
TEST_F(MemoryResourceManagerTest, ZeroUsage) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant = tenantManager.getTenant("mem_tenant1");
    memManager.allocateMemoryResource(tenant);
    
    memManager.updateMemoryUsage("mem_tenant1", 0.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("mem_tenant1"), 0.0);
}

/**
 * @brief 测试不存在租户的内存操作
 */
TEST_F(MemoryResourceManagerTest, NonExistentTenant) {
    auto& memManager = MemoryResourceManager::getInstance();
    
    memManager.initialize(8192);
    
    double usage = memManager.getTenantMemoryUsage("nonexistent_tenant");
    EXPECT_EQ(usage, 0.0);
}
