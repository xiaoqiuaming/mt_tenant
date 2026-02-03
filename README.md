# YaoBase 多租户资源隔离项目

这是一个实现YaoBase数据库多租户资源隔离功能的C++项目。

## 构建要求

- CMake 3.10+
- C++17 编译器
- Linux/Windows 环境

## 构建步骤

```bash
mkdir build
cd build
cmake ..
make
```

## 运行

```bash
./yaobase_tenant          # 运行演示
./yaobase_tenant --test   # 运行内置单元测试（已弃用，使用GoogleTest）
./yaobase_tenant --benchmark  # 运行性能基准测试
```

## 测试

项目使用GoogleTest框架进行全面的单元测试和集成测试。

### 运行测试

```bash
# 构建并运行所有测试
cd build
make
./tests/unit_tests          # 运行单元测试
./tests/integration_tests   # 运行集成测试

# 或使用ctest
ctest --output-on-failure
```

### 测试覆盖

- **单元测试** (90个测试用例)
  - 租户管理 (TenantContext, TenantManager, TenantAuthenticator)
  - 资源管理 (CPU, Memory, Disk)
  - 配额检查 (CpuQuotaChecker, MemoryQuotaChecker, DiskQuotaChecker)
  - 配置和工具类 (ConfigManager, RequestContext, BasicResourceStats)

- **集成测试**
  - 服务器组件集成 (SqlServer, DataServer, TransServer, AdminServer)
  - 多租户资源隔离验证

详细测试文档请参见 [TESTING.md](TESTING.md)

## 项目结构

```
src/
├── core/           # 核心组件
│   ├── tenant/     # 租户管理
│   └── resource/   # 资源管理
├── server/         # 服务器组件
│   ├── sql/        # SQL服务器
│   ├── data/       # 数据服务器
│   ├── trans/      # 事务服务器
│   └── admin/      # 管理服务器
├── common/         # 公共组件
│   ├── config/     # 配置管理
│   └── utils/      # 工具类
└── main.cpp        # 主程序

tests/              # 测试套件
├── unit/           # 单元测试
└── integration/    # 集成测试
```

## 功能特性

- 租户上下文管理
- CPU资源隔离（支持cgroup）
- 内存和磁盘配额控制
- 多服务器架构支持
- 实时监控和告警
- 全面的GoogleTest测试覆盖

## 开发指南

参见以下文档：
- [ARCHITECTURE.md](ARCHITECTURE.md) - 系统架构设计
- [CONTRIBUTING.md](CONTRIBUTING.md) - 贡献指南
- [TESTING.md](TESTING.md) - 测试指南