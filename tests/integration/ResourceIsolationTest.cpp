#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>
#include "core/tenant/TenantManager.h"
#include "core/resource/CpuResourceManager.h"
#include "core/resource/MemoryResourceManager.h"
#include "core/resource/DiskResourceManager.h"
#include "core/resource/CpuQuotaChecker.h"
#include "core/resource/MemoryQuotaChecker.h"
#include "core/resource/DiskQuotaChecker.h"
#include "common/config/ConfigManager.h"

using namespace yao;

/**
 * @brief 资源隔离集成测试类
 */
class ResourceIsolationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试租户
        auto& tenantManager = TenantManager::getInstance();
        tenantManager.createTenant("isolation_tenant1", 4, 
                                  4LL * 1024 * 1024 * 1024, 
                                  40LL * 1024 * 1024 * 1024);
        tenantManager.createTenant("isolation_tenant2", 2, 
                                  2LL * 1024 * 1024 * 1024, 
                                  20LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        // 清理测试租户
        auto& tenantManager = TenantManager::getInstance();
        tenantManager.removeTenant("isolation_tenant1");
        tenantManager.removeTenant("isolation_tenant2");
    }
};

/**
 * @brief 测试CPU资源隔离
 */
TEST_F(ResourceIsolationTest, CpuResourceIsolation) {
    auto& cpuManager = CpuResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    cpuManager.initializeCgroup(false);
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    ASSERT_NE(tenant1, nullptr);
    ASSERT_NE(tenant2, nullptr);
    
    // 为两个租户分配CPU资源
    EXPECT_TRUE(cpuManager.allocateCpuResource(tenant1));
    EXPECT_TRUE(cpuManager.allocateCpuResource(tenant2));
    
    // 模拟不同的CPU使用情况
    cpuManager.updateCpuUsage("isolation_tenant1", 0.3);
    cpuManager.updateCpuUsage("isolation_tenant2", 0.5);
    
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("isolation_tenant1"), 0.3);
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("isolation_tenant2"), 0.5);
    
    // 释放资源
    cpuManager.releaseCpuResource("isolation_tenant1");
    cpuManager.releaseCpuResource("isolation_tenant2");
}

/**
 * @brief 测试内存资源隔离
 */
TEST_F(ResourceIsolationTest, MemoryResourceIsolation) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    // 为两个租户分配内存资源
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant1));
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant2));
    
    // 租户1更新使用2GB
    memManager.updateMemoryUsage("isolation_tenant1", 2048.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant1"), 2048.0);
    
    // 租户2更新使用1GB
    memManager.updateMemoryUsage("isolation_tenant2", 1024.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant2"), 1024.0);
    
    // 验证租户间隔离
    EXPECT_NE(memManager.getTenantMemoryUsage("isolation_tenant1"), 
              memManager.getTenantMemoryUsage("isolation_tenant2"));
    
    // 清理
    memManager.releaseMemoryResource("isolation_tenant1");
    memManager.releaseMemoryResource("isolation_tenant2");
}

/**
 * @brief 测试磁盘资源隔离
 */
TEST_F(ResourceIsolationTest, DiskResourceIsolation) {
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    diskManager.initialize(100);
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    // 为两个租户分配磁盘资源
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant1));
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant2));
    
    // 租户1更新使用20GB
    diskManager.updateDiskUsage("isolation_tenant1", 20.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("isolation_tenant1"), 20.0);
    
    // 租户2更新使用10GB
    diskManager.updateDiskUsage("isolation_tenant2", 10.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("isolation_tenant2"), 10.0);
    
    // 验证租户间隔离
    EXPECT_NE(diskManager.getTenantDiskUsage("isolation_tenant1"), 
              diskManager.getTenantDiskUsage("isolation_tenant2"));
    
    // 清理
    diskManager.releaseDiskResource("isolation_tenant1");
    diskManager.releaseDiskResource("isolation_tenant2");
}

/**
 * @brief 测试综合资源隔离
 */
TEST_F(ResourceIsolationTest, ComprehensiveResourceIsolation) {
    auto& cpuManager = CpuResourceManager::getInstance();
    auto& memManager = MemoryResourceManager::getInstance();
    auto& diskManager = DiskResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    // 初始化所有资源管理器
    cpuManager.initializeCgroup(false);
    memManager.initialize(8192);
    diskManager.initialize(100);
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    ASSERT_NE(tenant1, nullptr);
    ASSERT_NE(tenant2, nullptr);
    
    // 为租户1分配资源
    EXPECT_TRUE(cpuManager.allocateCpuResource(tenant1));
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant1));
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant1));
    
    // 为租户2分配资源
    EXPECT_TRUE(cpuManager.allocateCpuResource(tenant2));
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant2));
    EXPECT_TRUE(diskManager.allocateDiskResource(tenant2));
    
    // 更新使用情况
    cpuManager.updateCpuUsage("isolation_tenant1", 0.4);
    memManager.updateMemoryUsage("isolation_tenant1", 2048.0);
    diskManager.updateDiskUsage("isolation_tenant1", 20.0);
    
    cpuManager.updateCpuUsage("isolation_tenant2", 0.2);
    memManager.updateMemoryUsage("isolation_tenant2", 1024.0);
    diskManager.updateDiskUsage("isolation_tenant2", 10.0);
    
    // 验证资源使用情况
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("isolation_tenant1"), 0.4);
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant1"), 2048.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("isolation_tenant1"), 20.0);
    
    EXPECT_DOUBLE_EQ(cpuManager.getTenantCpuUsage("isolation_tenant2"), 0.2);
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant2"), 1024.0);
    EXPECT_EQ(diskManager.getTenantDiskUsage("isolation_tenant2"), 10.0);
    
    // 清理资源
    cpuManager.releaseCpuResource("isolation_tenant1");
    cpuManager.releaseCpuResource("isolation_tenant2");
    memManager.releaseMemoryResource("isolation_tenant1");
    memManager.releaseMemoryResource("isolation_tenant2");
    diskManager.releaseDiskResource("isolation_tenant1");
    diskManager.releaseDiskResource("isolation_tenant2");
}

/**
 * @brief 测试配额检查器协同工作
 */
TEST_F(ResourceIsolationTest, QuotaCheckersWorkTogether) {
    CpuQuotaChecker cpuChecker;
    auto& memChecker = MemoryQuotaChecker::getInstance();
    auto& diskChecker = DiskQuotaChecker::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    ASSERT_NE(tenant1, nullptr);
    ASSERT_NE(tenant2, nullptr);
    
    // 租户1在配额内
    cpuChecker.updateUsage("isolation_tenant1", 0.3);  // 30%，配额40%
    EXPECT_TRUE(cpuChecker.checkQuota("isolation_tenant1"));
    EXPECT_TRUE(memChecker.checkQuota(tenant1, 3072.0));  // 3GB，配额4GB
    EXPECT_TRUE(diskChecker.checkQuota(tenant1, 30.0)); // 30GB，配额40GB
    
    // 租户2超配额
    cpuChecker.updateUsage("isolation_tenant2", 0.25);  // 25%，配额20%
    EXPECT_FALSE(cpuChecker.checkQuota("isolation_tenant2"));
    EXPECT_FALSE(memChecker.checkQuota(tenant2, 3072.0));  // 3GB，配额2GB
    EXPECT_FALSE(diskChecker.checkQuota(tenant2, 25.0)); // 25GB，配额20GB
}

/**
 * @brief 测试租户间资源竞争
 */
TEST_F(ResourceIsolationTest, TenantResourceContention) {
    auto& memManager = MemoryResourceManager::getInstance();
    auto& tenantManager = TenantManager::getInstance();
    
    memManager.initialize(8192);
    
    auto tenant1 = tenantManager.getTenant("isolation_tenant1");
    auto tenant2 = tenantManager.getTenant("isolation_tenant2");
    
    // 两个租户同时分配资源
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant1));
    EXPECT_TRUE(memManager.allocateMemoryResource(tenant2));
    
    // 租户1使用3GB（在配额内）
    memManager.updateMemoryUsage("isolation_tenant1", 3072.0);
    
    // 租户2使用1GB（在配额内）
    memManager.updateMemoryUsage("isolation_tenant2", 1024.0);
    
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant1"), 3072.0);
    EXPECT_EQ(memManager.getTenantMemoryUsage("isolation_tenant2"), 1024.0);
    
    // 清理
    memManager.releaseMemoryResource("isolation_tenant1");
    memManager.releaseMemoryResource("isolation_tenant2");
}
