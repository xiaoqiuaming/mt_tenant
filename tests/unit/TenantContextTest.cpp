#include <gtest/gtest.h>
#include "core/tenant/TenantContext.h"

using namespace yao;

/**
 * @brief TenantContext 单元测试类
 */
class TenantContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前准备
    }

    void TearDown() override {
        // 测试后清理
    }
};

/**
 * @brief 测试TenantContext构造函数
 */
TEST_F(TenantContextTest, ConstructorInitializesCorrectly) {
    TenantContext tenant("test_tenant", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    
    EXPECT_EQ(tenant.getTenantId(), "test_tenant");
    EXPECT_EQ(tenant.getCpuQuota(), 2);
    EXPECT_EQ(tenant.getMemoryQuota(), 1024 * 1024 * 1024);
    EXPECT_EQ(tenant.getDiskQuota(), 10LL * 1024 * 1024 * 1024);
}

/**
 * @brief 测试设置CPU配额
 */
TEST_F(TenantContextTest, SetCpuQuota) {
    TenantContext tenant("test_tenant", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    
    tenant.setCpuQuota(4);
    EXPECT_EQ(tenant.getCpuQuota(), 4);
}

/**
 * @brief 测试设置内存配额
 */
TEST_F(TenantContextTest, SetMemoryQuota) {
    TenantContext tenant("test_tenant", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    
    size_t newMemory = 2LL * 1024 * 1024 * 1024;
    tenant.setMemoryQuota(newMemory);
    EXPECT_EQ(tenant.getMemoryQuota(), newMemory);
}

/**
 * @brief 测试设置磁盘配额
 */
TEST_F(TenantContextTest, SetDiskQuota) {
    TenantContext tenant("test_tenant", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    
    size_t newDisk = 20LL * 1024 * 1024 * 1024;
    tenant.setDiskQuota(newDisk);
    EXPECT_EQ(tenant.getDiskQuota(), newDisk);
}

/**
 * @brief 测试零配额
 */
TEST_F(TenantContextTest, ZeroQuota) {
    TenantContext tenant("test_tenant", 0, 0, 0);
    
    EXPECT_EQ(tenant.getCpuQuota(), 0);
    EXPECT_EQ(tenant.getMemoryQuota(), 0);
    EXPECT_EQ(tenant.getDiskQuota(), 0);
}

/**
 * @brief 测试大配额值
 */
TEST_F(TenantContextTest, LargeQuota) {
    size_t largeMemory = 100LL * 1024 * 1024 * 1024;  // 100GB
    size_t largeDisk = 1024LL * 1024 * 1024 * 1024;   // 1TB
    
    TenantContext tenant("test_tenant", 16, largeMemory, largeDisk);
    
    EXPECT_EQ(tenant.getCpuQuota(), 16);
    EXPECT_EQ(tenant.getMemoryQuota(), largeMemory);
    EXPECT_EQ(tenant.getDiskQuota(), largeDisk);
}
