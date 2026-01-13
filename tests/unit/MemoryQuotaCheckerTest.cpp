#include <gtest/gtest.h>
#include "core/resource/MemoryQuotaChecker.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief MemoryQuotaChecker 单元测试类
 */
class MemoryQuotaCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("mem_check_tenant");
        manager.createTenant("mem_check_tenant", 2, 2LL * 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("mem_check_tenant");
    }
};

/**
 * @brief 测试未超配额的检查
 */
TEST_F(MemoryQuotaCheckerTest, CheckQuotaWithinLimit) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 1024.0; // 1GB，在2GB配额内
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试超配额的检查
 */
TEST_F(MemoryQuotaCheckerTest, CheckQuotaExceeded) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 3072.0; // 3GB，超过2GB配额
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_FALSE(result);
}

/**
 * @brief 测试边界情况 - 恰好达到配额
 */
TEST_F(MemoryQuotaCheckerTest, CheckQuotaAtBoundary) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    double usage = 2048.0; // 2GB，恰好等于配额
    bool result = checker.checkQuota(tenant, usage);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试零使用量
 */
TEST_F(MemoryQuotaCheckerTest, ZeroUsage) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    bool result = checker.checkQuota(tenant, 0.0);
    EXPECT_TRUE(result);
}

/**
 * @brief 测试多次检查
 */
TEST_F(MemoryQuotaCheckerTest, MultipleChecks) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    EXPECT_TRUE(checker.checkQuota(tenant, 512.0));
    EXPECT_TRUE(checker.checkQuota(tenant, 1024.0));
    EXPECT_TRUE(checker.checkQuota(tenant, 1536.0));
    EXPECT_FALSE(checker.checkQuota(tenant, 3072.0));
}

/**
 * @brief 测试小内存量
 */
TEST_F(MemoryQuotaCheckerTest, SmallMemoryAmounts) {
    auto& checker = MemoryQuotaChecker::getInstance();
    auto& manager = TenantManager::getInstance();
    
    auto tenant = manager.getTenant("mem_check_tenant");
    ASSERT_NE(tenant, nullptr);
    
    EXPECT_TRUE(checker.checkQuota(tenant, 0.001));  // 1KB
    EXPECT_TRUE(checker.checkQuota(tenant, 1.0));    // 1MB
}
