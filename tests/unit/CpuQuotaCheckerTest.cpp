#include <gtest/gtest.h>
#include "core/resource/CpuQuotaChecker.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief CpuQuotaChecker 单元测试类
 */
class CpuQuotaCheckerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试租户
        auto& manager = TenantManager::getInstance();
        manager.createTenant("quota_tenant", 50, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
        manager.createTenant("low_quota_tenant", 10, 512 * 1024 * 1024, 5LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        // 清理测试租户
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("quota_tenant");
        manager.removeTenant("low_quota_tenant");
    }
};

/**
 * @brief 测试未超配额的检查
 */
TEST_F(CpuQuotaCheckerTest, CheckQuotaWithinLimit) {
    CpuQuotaChecker checker;
    
    // 初始状态应该在配额内
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试超配额的检查
 */
TEST_F(CpuQuotaCheckerTest, CheckQuotaExceeded) {
    CpuQuotaChecker checker;
    
    // 更新使用率到0.6（超过50%的配额）
    checker.updateUsage("quota_tenant", 0.6);
    
    // 应该检测到超配额
    EXPECT_FALSE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试边界情况 - 恰好达到配额
 */
TEST_F(CpuQuotaCheckerTest, CheckQuotaAtBoundary) {
    CpuQuotaChecker checker;
    
    // 使用率恰好等于配额
    checker.updateUsage("quota_tenant", 0.50);
    
    // 边界情况应该通过检查
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试多次更新使用率
 */
TEST_F(CpuQuotaCheckerTest, MultipleUsageUpdates) {
    CpuQuotaChecker checker;
    
    // 第一次更新
    checker.updateUsage("quota_tenant", 0.3);
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
    
    // 第二次更新
    checker.updateUsage("quota_tenant", 0.45);
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
    
    // 第三次更新，超配额
    checker.updateUsage("quota_tenant", 0.7);
    EXPECT_FALSE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试不存在的租户
 */
TEST_F(CpuQuotaCheckerTest, NonExistentTenant) {
    CpuQuotaChecker checker;
    
    // 不存在的租户应该返回false
    EXPECT_FALSE(checker.checkQuota("nonexistent_tenant"));
}

/**
 * @brief 测试低配额租户
 */
TEST_F(CpuQuotaCheckerTest, LowQuotaTenant) {
    CpuQuotaChecker checker;
    
    // 10%的配额，使用率0.05应该在配额内
    checker.updateUsage("low_quota_tenant", 0.05);
    EXPECT_TRUE(checker.checkQuota("low_quota_tenant"));
    
    // 使用率0.15应该超配额
    checker.updateUsage("low_quota_tenant", 0.15);
    EXPECT_FALSE(checker.checkQuota("low_quota_tenant"));
}

/**
 * @brief 测试零使用率
 */
TEST_F(CpuQuotaCheckerTest, ZeroUsage) {
    CpuQuotaChecker checker;
    
    checker.updateUsage("quota_tenant", 0.0);
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试100%使用率
 */
TEST_F(CpuQuotaCheckerTest, MaxUsage) {
    CpuQuotaChecker checker;
    
    checker.updateUsage("quota_tenant", 1.0);
    EXPECT_FALSE(checker.checkQuota("quota_tenant"));
}

/**
 * @brief 测试多租户配额检查
 */
TEST_F(CpuQuotaCheckerTest, MultiTenantQuotaCheck) {
    CpuQuotaChecker checker;
    
    // 租户1使用30%
    checker.updateUsage("quota_tenant", 0.3);
    EXPECT_TRUE(checker.checkQuota("quota_tenant"));
    
    // 租户2使用5%
    checker.updateUsage("low_quota_tenant", 0.05);
    EXPECT_TRUE(checker.checkQuota("low_quota_tenant"));
    
    // 租户1超配额不影响租户2
    checker.updateUsage("quota_tenant", 0.8);
    EXPECT_FALSE(checker.checkQuota("quota_tenant"));
    EXPECT_TRUE(checker.checkQuota("low_quota_tenant"));
}
