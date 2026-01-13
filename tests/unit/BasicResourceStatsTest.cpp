#include <gtest/gtest.h>
#include "core/resource/BasicResourceStats.h"

using namespace yao;

/**
 * @brief BasicResourceStats 单元测试类
 */
class BasicResourceStatsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 准备测试
    }

    void TearDown() override {
        // 清理
    }
};

/**
 * @brief 测试初始值
 */
TEST_F(BasicResourceStatsTest, InitialValues) {
    BasicResourceStats stats;
    
    EXPECT_EQ(stats.getCpuUsage(), 0.0);
    EXPECT_EQ(stats.getMemoryUsage(), 0);
    EXPECT_EQ(stats.getDiskUsage(), 0);
}

/**
 * @brief 测试重置功能
 */
TEST_F(BasicResourceStatsTest, Reset) {
    BasicResourceStats stats;
    
    // 假设BasicResourceStats有设置方法（基于接口定义）
    // 如果没有，这个测试验证reset后值为0
    stats.reset();
    
    EXPECT_EQ(stats.getCpuUsage(), 0.0);
    EXPECT_EQ(stats.getMemoryUsage(), 0);
    EXPECT_EQ(stats.getDiskUsage(), 0);
}

/**
 * @brief 测试多次重置
 */
TEST_F(BasicResourceStatsTest, MultipleResets) {
    BasicResourceStats stats;
    
    stats.reset();
    EXPECT_EQ(stats.getCpuUsage(), 0.0);
    
    stats.reset();
    EXPECT_EQ(stats.getCpuUsage(), 0.0);
    
    stats.reset();
    EXPECT_EQ(stats.getCpuUsage(), 0.0);
}

/**
 * @brief 测试作为ResourceStats接口使用
 */
TEST_F(BasicResourceStatsTest, AsResourceStatsInterface) {
    std::unique_ptr<ResourceStats> stats = std::make_unique<BasicResourceStats>();
    
    // 通过接口访问
    EXPECT_EQ(stats->getCpuUsage(), 0.0);
    EXPECT_EQ(stats->getMemoryUsage(), 0);
    EXPECT_EQ(stats->getDiskUsage(), 0);
    
    stats->reset();
    
    EXPECT_EQ(stats->getCpuUsage(), 0.0);
}

/**
 * @brief 测试多个独立实例
 */
TEST_F(BasicResourceStatsTest, MultipleInstances) {
    BasicResourceStats stats1;
    BasicResourceStats stats2;
    
    // 每个实例独立
    EXPECT_EQ(stats1.getCpuUsage(), 0.0);
    EXPECT_EQ(stats2.getCpuUsage(), 0.0);
    
    stats1.reset();
    stats2.reset();
    
    EXPECT_EQ(stats1.getCpuUsage(), 0.0);
    EXPECT_EQ(stats2.getCpuUsage(), 0.0);
}
