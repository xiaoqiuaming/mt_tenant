#include <gtest/gtest.h>
#include "common/config/ConfigManager.h"

using namespace yao;

/**
 * @brief ConfigManager 单元测试类
 */
class ConfigManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 清理之前的配置
        auto& config = ConfigManager::getInstance();
    }

    void TearDown() override {
        // 测试后无需特殊清理，因为是单例
    }
};

/**
 * @brief 测试设置和获取字符串配置
 */
TEST_F(ConfigManagerTest, SetAndGetString) {
    auto& config = ConfigManager::getInstance();
    
    config.setString("test_key", "test_value");
    std::string value = config.getString("test_key");
    
    EXPECT_EQ(value, "test_value");
}

/**
 * @brief 测试获取不存在的字符串配置
 */
TEST_F(ConfigManagerTest, GetNonExistentString) {
    auto& config = ConfigManager::getInstance();
    
    std::string value = config.getString("nonexistent_key", "default");
    EXPECT_EQ(value, "default");
}

/**
 * @brief 测试设置和获取整数配置
 */
TEST_F(ConfigManagerTest, SetAndGetInt) {
    auto& config = ConfigManager::getInstance();
    
    config.setInt("int_key", 42);
    int value = config.getInt("int_key");
    
    EXPECT_EQ(value, 42);
}

/**
 * @brief 测试获取不存在的整数配置
 */
TEST_F(ConfigManagerTest, GetNonExistentInt) {
    auto& config = ConfigManager::getInstance();
    
    int value = config.getInt("nonexistent_int", 100);
    EXPECT_EQ(value, 100);
}

/**
 * @brief 测试设置和获取布尔配置
 */
TEST_F(ConfigManagerTest, SetAndGetBool) {
    auto& config = ConfigManager::getInstance();
    
    config.setBool("bool_key", true);
    bool value = config.getBool("bool_key");
    
    EXPECT_TRUE(value);
    
    config.setBool("bool_key", false);
    value = config.getBool("bool_key");
    
    EXPECT_FALSE(value);
}

/**
 * @brief 测试获取不存在的布尔配置
 */
TEST_F(ConfigManagerTest, GetNonExistentBool) {
    auto& config = ConfigManager::getInstance();
    
    bool value = config.getBool("nonexistent_bool", true);
    EXPECT_TRUE(value);
}

/**
 * @brief 测试覆盖配置值
 */
TEST_F(ConfigManagerTest, OverwriteValue) {
    auto& config = ConfigManager::getInstance();
    
    config.setString("key", "value1");
    EXPECT_EQ(config.getString("key"), "value1");
    
    config.setString("key", "value2");
    EXPECT_EQ(config.getString("key"), "value2");
}

/**
 * @brief 测试整数边界值
 */
TEST_F(ConfigManagerTest, IntBoundaryValues) {
    auto& config = ConfigManager::getInstance();
    
    config.setInt("zero", 0);
    EXPECT_EQ(config.getInt("zero"), 0);
    
    config.setInt("negative", -100);
    EXPECT_EQ(config.getInt("negative"), -100);
    
    config.setInt("large", 1000000);
    EXPECT_EQ(config.getInt("large"), 1000000);
}

/**
 * @brief 测试空字符串
 */
TEST_F(ConfigManagerTest, EmptyString) {
    auto& config = ConfigManager::getInstance();
    
    config.setString("empty", "");
    EXPECT_EQ(config.getString("empty"), "");
}

/**
 * @brief 测试多个配置项
 */
TEST_F(ConfigManagerTest, MultipleConfigs) {
    auto& config = ConfigManager::getInstance();
    
    config.setString("str1", "value1");
    config.setString("str2", "value2");
    config.setInt("int1", 10);
    config.setInt("int2", 20);
    config.setBool("bool1", true);
    config.setBool("bool2", false);
    
    EXPECT_EQ(config.getString("str1"), "value1");
    EXPECT_EQ(config.getString("str2"), "value2");
    EXPECT_EQ(config.getInt("int1"), 10);
    EXPECT_EQ(config.getInt("int2"), 20);
    EXPECT_TRUE(config.getBool("bool1"));
    EXPECT_FALSE(config.getBool("bool2"));
}
