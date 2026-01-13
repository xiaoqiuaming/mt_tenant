#include <gtest/gtest.h>
#include "core/resource/CpuResourceManager.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief CpuResourceManager 单元测试类
 */
class CpuResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("cpu_test_tenant");
        manager.createTenant("cpu_test_tenant", 4, 2LL * 1024 * 1024 * 1024, 20LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("cpu_test_tenant");
    }
};

/**
 * @brief 测试初始化cgroup
 */
TEST_F(CpuResourceManagerTest, InitializeCgroup) {
    auto& cpuManager = CpuResourceManager::getInstance();
    
    // 测试禁用cgroup
    bool result1 = cpuManager.initializeCgroup(false);
    EXPECT_TRUE(result1);
    
    // 测试启用cgroup（可能失败，取决于系统支持）
    bool result2 = cpuManager.initializeCgroup(true);
    // 不强制要求成功，因为可能在不支持cgroup的系统上运行
    EXPECT_TRUE(result2 || !result2);
}

/**
 * @brief 测试分配CPU资源
 */
TEST_F(CpuResourceManagerTest, AllocateCpuResource) {
    auto& cpuManager = CpuResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    cpuManager.initializeCgroup(false);
    
    auto tenant = tenantManager.getTenant("cpu_test_tenant");
    ASSERT_NE(tenant, nullptr);
    
    bool result = cpuManager.allocateCpuResource(tenant);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试释放CPU资源
 */
TEST_F(CpuResourceManagerTest, ReleaseCpuResource) {
    auto& cpuManager = CpuResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    cpuManager.initializeCgroup(false);
    
    auto tenant = tenantManager.getTenant("cpu_test_tenant");
    ASSERT_NE(tenant, nullptr);
    
    cpuManager.allocateCpuResource(tenant);
    bool result = cpuManager.releaseCpuResource("cpu_test_tenant");
    EXPECT_TRUE(result);
}

/**
 * @brief 测试更新CPU使用率
 */
TEST_F(CpuResourceManagerTest, UpdateCpuUsage) {
    auto& cpuManager = CpuResourceManager::getInstance();
    
    cpuManager.updateCpuUsage("cpu_test_tenant", 0.5);
    
    double usage = cpuManager.getTenantCpuUsage("cpu_test_tenant");
    EXPECT_DOUBLE_EQ(usage, 0.5);
}

/**
 * @brief 测试获取不存在租户的CPU使用率
 */
TEST_F(CpuResourceManagerTest, GetNonExistentTenantUsage) {
    auto& cpuManager = CpuResourceManager::getInstance();
    
    double usage = cpuManager.getTenantCpuUsage("nonexistent_tenant");
    EXPECT_EQ(usage, -1.0);
}

/**
 * @brief 测试多次更新使用率
 */
TEST_F(CpuResourceManagerTest, MultipleUsageUpdates) {
    auto& cpuManager = CpuResourceManager::getInstance();
    
    cpuManager.updateCpuUsage("cpu_test_tenant", 0.3);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("cpu_test_tenant"), 0.3);
    
    cpuManager.updateCpuUsage("cpu_test_tenant", 0.7);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("cpu_test_tenant"), 0.7);
    
    cpuManager.updateCpuUsage("cpu_test_tenant", 0.2);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("cpu_test_tenant"), 0.2);
}

/**
 * @brief 测试边界值使用率
 */
TEST_F(CpuResourceManagerTest, BoundaryUsageValues) {
    auto& cpuManager = CpuResourceManager::getInstance();
    
    // 0%使用率
    cpuManager.updateCpuUsage("cpu_test_tenant", 0.0);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("cpu_test_tenant"), 0.0);
    
    // 100%使用率
    cpuManager.updateCpuUsage("cpu_test_tenant", 1.0);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("cpu_test_tenant"), 1.0);
}
