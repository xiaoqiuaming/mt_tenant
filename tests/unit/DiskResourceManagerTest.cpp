#include <gtest/gtest.h>
#include "core/resource/DiskResourceManager.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief DiskResourceManager 单元测试类
 */
class DiskResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("disk_tenant1");
        manager.removeTenant("disk_tenant2");
        manager.createTenant("disk_tenant1", 2, 2LL * 1024 * 1024 * 1024, 20LL * 1024 * 1024 * 1024);
        manager.createTenant("disk_tenant2", 2, 2LL * 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("disk_tenant1");
        manager.removeTenant("disk_tenant2");
    }
};

/**
 * @brief 测试初始化磁盘管理器
 */
TEST_F(DiskResourceManagerTest, Initialize) {
    auto& diskManager = DiskResourceManager::getInstance();
    
    size_t totalDiskGB = 100;
    bool result = diskManager.initialize(totalDiskGB);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试分配磁盘资源
 */
TEST_F(DiskResourceManagerTest, AllocateDiskResource) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    ASSERT_NE(tenant, nullptr);
    
    bool result = diskManager.allocateDiskResource(tenant);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试释放磁盘资源
 */
TEST_F(DiskResourceManagerTest, ReleaseDiskResource) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    diskManager.releaseDiskResource("disk_tenant1");
    
    // 验证释放后使用率为0
    double usage = diskManager.getTenantDiskUsage("disk_tenant1");
    EXPECT_EQ(usage, 0.0);
}

/**
 * @brief 测试获取磁盘使用量
 */
TEST_F(DiskResourceManagerTest, GetDiskUsage) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    // 更新使用情况
    diskManager.updateDiskUsage("disk_tenant1", 5.0); // 5GB
    
    double usage = diskManager.getTenantDiskUsage("disk_tenant1");
    EXPECT_EQ(usage, 5.0);
}

/**
 * @brief 测试检查磁盘配额
 */
TEST_F(DiskResourceManagerTest, CheckDiskQuota) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    // 租户配额20GB，请求10GB应该成功
    EXPECT_TRUE(diskManager.checkDiskQuota("disk_tenant1", 10.0));
    
    // 请求25GB应该失败
    EXPECT_FALSE(diskManager.checkDiskQuota("disk_tenant1", 25.0));
}

/**
 * @brief 测试多次更新使用量
 */
TEST_F(DiskResourceManagerTest, MultipleUsageUpdates) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    diskManager.updateDiskUsage("disk_tenant1", 5.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant1"), 5.0);
    
    diskManager.updateDiskUsage("disk_tenant1", 10.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant1"), 10.0);
    
    diskManager.updateDiskUsage("disk_tenant1", 3.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant1"), 3.0);
}

/**
 * @brief 测试多租户磁盘分配
 */
TEST_F(DiskResourceManagerTest, MultiTenantAllocation) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant1 = tenantManager.getTenant("disk_tenant1");
    auto tenant2 = tenantManager.getTenant("disk_tenant2");
    
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant1));
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant2));
    
    diskManager.updateDiskUsage("disk_tenant1", 10.0);
    diskManager.updateDiskUsage("disk_tenant2", 5.0);
    
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant1"), 10.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant2"), 5.0);
}

/**
 * @brief 测试零使用量
 */
TEST_F(DiskResourceManagerTest, ZeroUsage) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    diskManager.updateDiskUsage("disk_tenant1", 0.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("disk_tenant1"), 0.0);
}

/**
 * @brief 测试不存在租户的磁盘操作
 */
TEST_F(DiskResourceManagerTest, NonExistentTenant) {
    auto& diskManager = DiskResourceManager::getInstance();
    
    diskManager.initialize(100);
    
    double usage = diskManager.getTenantDiskUsage("nonexistent_tenant");
    EXPECT_EQ(usage, 0.0);
}

/**
 * @brief 测试逐步接近配额限制
 */
TEST_F(DiskResourceManagerTest, GradualApproachToQuota) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant = tenantManager.getTenant("disk_tenant1");
    diskManager.allocateDiskResource(tenant);
    
    // 租户配额20GB
    EXPECT_TRUE(diskManager.checkDiskQuota("disk_tenant1", 10.0));
    EXPECT_TRUE(diskManager.checkDiskQuota("disk_tenant1", 15.0));
    EXPECT_TRUE(diskManager.checkDiskQuota("disk_tenant1", 20.0));
    EXPECT_FALSE(diskManager.checkDiskQuota("disk_tenant1", 25.0));
}
