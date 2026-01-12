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
./yaobase_tenant --test   # 运行单元测试
./yaobase_tenant --benchmark  # 运行性能基准测试
```

## 测试

项目包含单元测试和性能基准测试：

- **单元测试** (`--test`)：测试租户管理、认证和配额检查功能
- **性能测试** (`--benchmark`)：模拟高并发负载，测量请求处理性能

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
```

## 功能特性

- 租户上下文管理
- CPU资源隔离（支持cgroup）
- 内存和磁盘配额控制
- 多服务器架构支持
- 实时监控和告警
- 单元测试和性能基准测试