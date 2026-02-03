#include <gtest/gtest.h>
#include "core/resource/DiskQuotaChecker.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief DiskQuotaChecker 单元测试类
 */
class DiskQuotaCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("disk_check_tenant");
        manager.createTenant("disk_check_tenant", 2, 2LL * 1024 * 1024 * 1024, 20LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("disk_check_tenant");
    }
};

/**
 * @brief 测试未超配额的检查
 */
TEST_F(DiskQuotaCheckerTest, CheckQuotaWithinLimit) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 10.0; // 10GB，在20GB配额内
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试超配额的检查
 */
TEST_F(DiskQuotaCheckerTest, CheckQuotaExceeded) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 25.0; // 25GB，超过20GB配额
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_FALSE(result);
}

/**
 * @brief 测试边界情况 - 恰好达到配额
 */
TEST_F(DiskQuotaCheckerTest, CheckQuotaAtBoundary) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 20.0; // 20GB，恰好等于配额
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试零使用量
 */
TEST_F(DiskQuotaCheckerTest, ZeroUsage) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    bool result = checker.checkQuota(tenant, 0.0);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试多次检查
 */
TEST_F(DiskQuotaCheckerTest, MultipleChecks) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    EXPECT_TRUE(checker.checkQuota(tenant, 5.0));
    EXPECT_TRUE(checker.checkQuota(tenant, 10.0));
    EXPECT_TRUE(checker.checkQuota(tenant, 15.0));
    EXPECT_FALSE(checker.checkQuota(tenant, 30.0));
}

/**
 * @brief 测试小磁盘量
 */
TEST_F(DiskQuotaCheckerTest, SmallDiskAmounts) {
    auto& checker = DiskQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("disk_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    EXPECT_TRUE(checker.checkQuota(tenant, 0.001));  // 1MB
    EXPECT_TRUE(checker.checkQuota(tenant, 1.0));    // 1GB
}
