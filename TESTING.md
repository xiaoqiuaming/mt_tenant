# GoogleTest集成说明

## 概述

本项目已集成GoogleTest测试框架，提供了全面的单元测试和集成测试。测试覆盖了核心组件、资源管理和服务器功能。

## 测试架构

### 目录结构
```
tests/
├── CMakeLists.txt           # 测试构建配置
├── unit/                    # 单元测试
│   ├── TenantContextTest.cpp
│   ├── TenantManagerTest.cpp
│   ├── TenantAuthenticatorTest.cpp
│   ├── CpuResourceManagerTest.cpp
│   ├── MemoryResourceManagerTest.cpp
│   ├── DiskResourceManagerTest.cpp
│   ├── CpuQuotaCheckerTest.cpp
│   ├── MemoryQuotaCheckerTest.cpp
│   ├── DiskQuotaCheckerTest.cpp
│   ├── ConfigManagerTest.cpp
│   ├── RequestContextTest.cpp
│   └── BasicResourceStatsTest.cpp
└── integration/             # 集成测试
    ├── ServerIntegrationTest.cpp
    └── ResourceIsolationTest.cpp
```

### 测试组件说明

#### 单元测试
- **TenantContextTest**: 测试租户上下文的创建、配额管理
- **TenantManagerTest**: 测试租户管理器的CRUD操作
- **TenantAuthenticatorTest**: 测试租户认证功能
- **CpuResourceManagerTest**: 测试CPU资源管理和cgroup集成
- **MemoryResourceManagerTest**: 测试内存资源分配和监控
- **DiskResourceManagerTest**: 测试磁盘资源管理
- **CpuQuotaCheckerTest**: 测试CPU配额检查逻辑
- **MemoryQuotaCheckerTest**: 测试内存配额检查
- **DiskQuotaCheckerTest**: 测试磁盘配额检查
- **ConfigManagerTest**: 测试配置管理功能
- **RequestContextTest**: 测试请求上下文管理
- **BasicResourceStatsTest**: 测试资源统计接口

#### 集成测试
- **ServerIntegrationTest**: 测试服务器组件的初始化、启动和请求处理
- **ResourceIsolationTest**: 测试多租户资源隔离功能

## 构建和运行测试

### 前提条件
- CMake 3.10+
- C++17兼容编译器
- 互联网连接（首次构建时下载GoogleTest）

### 构建步骤

#### Linux/Mac
```bash
# 创建构建目录
mkdir build
cd build

# 配置CMake
cmake ..

# 编译项目和测试
make -j$(nproc)

# 运行单元测试
./tests/unit_tests

# 运行集成测试
./tests/integration_tests

# 使用ctest运行所有测试
ctest --output-on-failure
```

#### Windows (使用PowerShell)
```powershell
# 创建构建目录
mkdir build
cd build

# 配置CMake
cmake ..

# 编译项目和测试
cmake --build . --config Release

# 运行单元测试
.\tests\Release\unit_tests.exe

# 运行集成测试
.\tests\Release\integration_tests.exe

# 使用ctest运行所有测试
ctest -C Release --output-on-failure
```

### 运行特定测试

```bash
# 运行特定测试套件
./tests/unit_tests --gtest_filter="TenantContextTest.*"

# 运行特定测试用例
./tests/unit_tests --gtest_filter="TenantManagerTest.CreateTenant"

# 显示所有测试名称
./tests/unit_tests --gtest_list_tests

# 运行测试并生成详细输出
./tests/unit_tests --gtest_verbose
```

## 测试最佳实践

### 编写新测试

1. **单元测试模板**:
```cpp
#include <gtest/gtest.h>
#include "your/component.h"

using namespace yao;

class YourComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前准备
    }

    void TearDown() override {
        // 测试后清理
    }
};

TEST_F(YourComponentTest, TestName) {
    // 测试代码
    EXPECT_EQ(expected, actual);
}
```

2. **集成测试模板**:
```cpp
#include <gtest/gtest.h>
#include <memory>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化多个组件
    }

    void TearDown() override {
        // 清理所有组件
    }
};

TEST_F(IntegrationTest, TestInteraction) {
    // 测试组件间交互
}
```

### 断言使用指南

- **EXPECT_EQ(a, b)**: 期望a等于b（失败后继续执行）
- **ASSERT_EQ(a, b)**: 断言a等于b（失败后终止测试）
- **EXPECT_TRUE(condition)**: 期望条件为真
- **EXPECT_NE(a, b)**: 期望a不等于b
- **EXPECT_LT/LE/GT/GE**: 比较运算符
- **EXPECT_DOUBLE_EQ(a, b)**: 浮点数比较
- **EXPECT_THROW/NO_THROW**: 异常测试

### 测试隔离原则

1. **每个测试独立**: 测试不应依赖其他测试的状态
2. **清理资源**: 在TearDown中清理所有创建的资源
3. **使用Mock对象**: 隔离外部依赖
4. **避免硬编码**: 使用常量和配置
5. **测试单一功能**: 每个测试只验证一个功能点

## 测试覆盖率

目前测试覆盖情况：
- 核心租户管理: ✓
- 资源管理器: ✓
- 配额检查器: ✓
- 配置管理: ✓
- 请求上下文: ✓
- 服务器组件: ✓

未来待增强：
- 线程池管理测试
- Cgroup控制器测试
- 监控组件测试
- 性能基准测试

## 持续集成

### GitHub Actions集成示例

```yaml
name: Tests
on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: |
          mkdir build
          cd build
          cmake ..
          make -j$(nproc)
      - name: Run Tests
        run: |
          cd build
          ctest --output-on-failure
```

## 故障排查

### 常见问题

1. **GoogleTest下载失败**
   - 检查网络连接
   - 设置HTTP代理: `export https_proxy=...`

2. **链接错误**
   - 确保C++17标准已启用
   - 检查pthread库链接（Linux）

3. **测试失败**
   - 检查日志输出了解详细原因
   - 确认测试环境满足前提条件
   - 使用`--gtest_verbose`获取详细信息

4. **内存/资源泄漏**
   - 使用Valgrind检测: `valgrind --leak-check=full ./tests/unit_tests`
   - 确保TearDown正确清理资源

## 性能测试

虽然当前主要是功能测试，但可以使用GoogleTest的性能测试功能：

```cpp
TEST_F(PerformanceTest, BenchmarkTenantCreation) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        // 性能测试代码
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000) << "Performance degradation detected";
}
```

## 贡献指南

### 添加新测试

1. 在适当的目录创建测试文件（unit/ 或 integration/）
2. 在tests/CMakeLists.txt中添加源文件
3. 遵循现有测试的命名和结构约定
4. 确保测试可重复运行
5. 添加适当的注释和文档

### 代码审查清单

- [ ] 测试覆盖所有功能路径
- [ ] 测试包含边界情况
- [ ] 测试使用有意义的断言消息
- [ ] 测试独立且可重复
- [ ] 测试运行时间合理（< 1秒）
- [ ] 资源正确清理
- [ ] 无硬编码值或魔数

## 参考资源

- [GoogleTest官方文档](https://google.github.io/googletest/)
- [GoogleTest入门指南](https://google.github.io/googletest/primer.html)
- [GoogleTest高级特性](https://google.github.io/googletest/advanced.html)
- [CMake与GoogleTest集成](https://cmake.org/cmake/help/latest/module/GoogleTest.html)

## 联系方式

如有问题或建议，请通过Issue或Pull Request与我们联系。
