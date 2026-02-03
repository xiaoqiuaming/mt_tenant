#include <gtest/gtest.h>
#include "core/resource/TenantAuthenticator.h"
#include "core/tenant/TenantManager.h"

using namespace yao;

/**
 * @brief TenantAuthenticator 单元测试类
 */
class TenantAuthenticatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试租户
        auto& manager = TenantManager::getInstance();
        manager.createTenant("auth_tenant", 2, 1024 * 1024 * 1024, 10LL * 1024 * 1024 * 1024);
        manager.createTenant("test_org", 4, 2LL * 1024 * 1024 * 1024, 20LL * 1024 * 1024 * 1024);
    }

    void TearDown() override {
        // 清理测试租户
        auto& manager = TenantManager::getInstance();
        manager.removeTenant("auth_tenant");
        manager.removeTenant("test_org");
    }
};

/**
 * @brief 测试有效的认证
 */
TEST_F(TenantAuthenticatorTest, ValidAuthentication) {
    TenantAuthenticator auth;
    
    std::string tenantId = auth.authenticate("user@auth_tenant", "password");
    EXPECT_EQ(tenantId, "auth_tenant");
}

/**
 * @brief 测试无效的认证格式
 */
TEST_F(TenantAuthenticatorTest, InvalidFormat) {
    TenantAuthenticator auth;
    
    // 没有@符号
    std::string tenantId1 = auth.authenticate("user", "password");
    EXPECT_EQ(tenantId1, "");
    
    // 空用户名
    std::string tenantId2 = auth.authenticate("@auth_tenant", "password");
    EXPECT_EQ(tenantId2, "");
    
    // 空字符串
    std::string tenantId3 = auth.authenticate("", "password");
    EXPECT_EQ(tenantId3, "");
}

/**
 * @brief 测试不存在的租户
 */
TEST_F(TenantAuthenticatorTest, NonExistentTenant) {
    TenantAuthenticator auth;
    
    std::string tenantId = auth.authenticate("user@nonexistent", "password");
    EXPECT_EQ(tenantId, "");
}

/**
 * @brief 测试多种用户名格式
 */
TEST_F(TenantAuthenticatorTest, VariousUsernameFormats) {
    TenantAuthenticator auth;
    
    // 简单用户名
    EXPECT_EQ(auth.authenticate("john@auth_tenant", "pass"), "auth_tenant");
    
    // 邮箱格式
    EXPECT_EQ(auth.authenticate("john.doe@auth_tenant", "pass"), "auth_tenant");
    
    // 带数字
    EXPECT_EQ(auth.authenticate("user123@auth_tenant", "pass"), "auth_tenant");
}

/**
 * @brief 测试多租户认证
 */
TEST_F(TenantAuthenticatorTest, MultiTenantAuthentication) {
    TenantAuthenticator auth;
    
    std::string tenant1 = auth.authenticate("user1@auth_tenant", "pass1");
    std::string tenant2 = auth.authenticate("user2@test_org", "pass2");
    
    EXPECT_EQ(tenant1, "auth_tenant");
    EXPECT_EQ(tenant2, "test_org");
    EXPECT_NE(tenant1, tenant2);
}

/**
 * @brief 测试相同租户的多个用户
 */
TEST_F(TenantAuthenticatorTest, MultipleUsersPerTenant) {
    TenantAuthenticator auth;
    
    std::string tenant1 = auth.authenticate("alice@auth_tenant", "pass1");
    std::string tenant2 = auth.authenticate("bob@auth_tenant", "pass2");
    std::string tenant3 = auth.authenticate("charlie@auth_tenant", "pass3");
    
    EXPECT_EQ(tenant1, "auth_tenant");
    EXPECT_EQ(tenant2, "auth_tenant");
    EXPECT_EQ(tenant3, "auth_tenant");
}

/**
 * @brief 测试空密码
 */
TEST_F(TenantAuthenticatorTest, EmptyPassword) {
    TenantAuthenticator auth;
    
    // 即使密码为空，只要租户存在且用户名格式正确，认证应该成功
    std::string tenantId = auth.authenticate("user@auth_tenant", "");
    EXPECT_EQ(tenantId, "auth_tenant");
}
