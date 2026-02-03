# YaoBase 多租户资源隔离系统 - 架构概览

## 系统层级架构

```mermaid
graph TB
    subgraph "客户端层"
        CLIENT[SQL Clients]
    end

    subgraph "服务层"
        SS[SqlServer]
        DS[DataServer]
        TS[TransServer]
        AS[AdminServer]
    end

    subgraph "核心组件层"
        TC[TenantContext]
        TM[TenantManager]
        CRM[CpuResourceManager]
        MRM[MemoryResourceManager]
        DRM[DiskResourceManager]
        TAU[TenantAuthenticator]
    end

    subgraph "通用组件层"
        CM[ConfigManager]
        UT[Utils]
    end

    CLIENT -->|SQL请求| SS
    SS -->|CPU隔离<br/>内存隔离| CRM
    SS -->|内存配额检查| MRM
    SS -->|租户认证| TAU
    SS -->|读取配置| CM
    
    SS -->|数据请求| DS
    DS -->|磁盘隔离| DRM
    
    SS -->|事务写入| TS
    AS -->|集群管理| TM
    
    CRM -->|获取配额| TC
    MRM -->|获取配额| TC
    TAU -->|验证身份| TC
    TM -->|管理租户| TC
```

## 请求处理数据流

```mermaid
sequenceDiagram
    participant Client as SQL Client
    participant SS as SqlServer
    participant Auth as TenantAuthenticator
    participant CPUMgr as CpuResourceManager
    participant MemMgr as MemoryResourceManager
    participant DS as DataServer
    participant TS as TransServer

    Client->>SS: 1. 发送SQL请求(user@tenant)
    SS->>Auth: 2. 验证租户身份
    Auth-->>SS: 3. 返回TenantContext
    
    SS->>CPUMgr: 4. 检查CPU配额
    CPUMgr-->>SS: 5. 返回配额状态
    
    SS->>MemMgr: 6. 检查内存配额
    MemMgr-->>SS: 7. 返回配额状态
    
    alt 资源充足
        SS->>DS: 8. 查询基线数据
        DS-->>SS: 9. 返回数据
        
        SS->>TS: 10. 查询增量数据
        TS-->>SS: 11. 返回数据
        
        SS->>SS: 12. 执行SQL逻辑
        SS-->>Client: 13. 返回结果
    else 资源不足
        SS-->>Client: 13. 拒绝请求
    end
```

## 租户资源隔离分层

```mermaid
graph LR
    subgraph SS1["SqlServer层隔离"]
        SS1A[CPU隔离<br/>线程池分配/cgroup]
        SS1B[内存隔离<br/>配额检查]
    end

    subgraph DS1["DataServer层隔离"]
        DS1A[磁盘隔离<br/>使用统计/配额控制]
    end

    subgraph TS1["TransServer共享"]
        TS1A[增量数据服务<br/>所有租户共享]
    end

    subgraph AS1["AdminServer共享"]
        AS1A[集群管理<br/>所有租户共享]
    end

    TenantA[Tenant A<br/>配额:4C 8G]
    TenantB[Tenant B<br/>配额:2C 4G]

    TenantA -->|分配| SS1A
    TenantA -->|分配| SS1B
    TenantA -->|使用| DS1A
    TenantA -->|使用| TS1A

    TenantB -->|分配| SS1A
    TenantB -->|分配| SS1B
    TenantB -->|使用| DS1A
    TenantB -->|使用| TS1A
```

## 核心组件详解 - CPU隔离

```mermaid
graph TB
    subgraph CPU["CPU隔离链路"]
        CQC["CpuQuotaChecker<br/>检查配额是否<br/>满足请求"]
        
        CRM["CpuResourceManager<br/>- 配额分配<br/>- 资源预留<br/>- 动态调整"]
        
        TPM["ThreadPoolManager<br/>- 租户线程组管理<br/>- 任务队列<br/>- 工作线程调度"]
        
        TTG["TenantThreadGroup<br/>- 租户专用线程组<br/>- 无锁任务队列<br/>- cgroup控制"]
        
        CGC["CgroupController<br/>- cgroup v1/v2支持<br/>- CPU份额分配<br/>- 进程PID管理"]
    end

    CQC -->|预检查| CRM
    CRM -->|分配线程| TPM
    TPM -->|创建/管理| TTG
    TTG -->|精确控制| CGC
```

## 核心组件详解 - 内存隔离

```mermaid
graph LR
    subgraph MEM["内存隔离链路"]
        MQC["MemoryQuotaChecker<br/>检查内存<br/>使用情况"]
        
        MRM["MemoryResourceManager<br/>- 配额分配<br/>- 使用统计<br/>- 超限处理"]
        
        SS["SqlServer<br/>- MemTable管理<br/>- 缓冲区池<br/>- 查询缓存"]
    end

    MQC -->|检查| MRM
    MRM -->|控制| SS
```

## 核心组件详解 - 磁盘隔离

```mermaid
graph LR
    subgraph DISK["磁盘隔离链路"]
        DQC["DiskQuotaChecker<br/>检查磁盘<br/>使用情况"]
        
        DRM["DiskResourceManager<br/>- 磁盘配额管理<br/>- SSTable统计<br/>- 超限禁写"]
        
        DS["DataServer<br/>- L1层管理<br/>- SSTable存储<br/>- 数据合并"]
    end

    DQC -->|检查| DRM
    DRM -->|控制| DS
```

## 服务器架构 - SqlServer

```mermaid
graph TB
    subgraph SQL["SqlServer"]
        CM["ConnectionManager<br/>- 连接建立<br/>- 连接路由<br/>- 连接关闭"]
        
        QP["Query Processor<br/>- SQL解析<br/>- 执行计划生成<br/>- 分布式执行"]
        
        RM["ResourceManager<br/>- CPU隔离<br/>- 内存隔离<br/>- 配额检查"]
        
        DM["DataAccess<br/>- 数据读写<br/>- 缓存管理<br/>- 事务协调"]
    end

    CM -->|认证| RM
    CM -->|路由| QP
    QP -->|检查| RM
    QP -->|访问| DM
```

## 工作线程模型

```mermaid
graph TB
    subgraph TP["全局线程池 120 threads"]
        TPA["Tenant A<br/>30 threads<br/>cpu.shares=1024"]
        TPB["Tenant B<br/>20 threads<br/>cpu.shares=768"]
        TPC["Tenant C<br/>20 threads<br/>cpu.shares=512"]
        SYS["System<br/>50 threads"]
    end

    subgraph CG["cgroup v1控制"]
        CGTA["cgroup-A<br/>cpu.shares"]
        CGTB["cgroup-B<br/>cpu.shares"]
        CGTC["cgroup-C<br/>cpu.shares"]
    end

    TPA -->|受限| CGTA
    TPB -->|受限| CGTB
    TPC -->|受限| CGTC
```

## 读取流程

```mermaid
graph LR
    C["客户端<br/>SELECT"]
    SS["SqlServer"]
    L1["DataServer<br/>L1层"]
    L0["TransServer<br/>L0层"]
    R["结果"]

    C -->|SQL查询| SS
    SS -->|查询基线| L1
    L1 -->|基线数据| SS
    SS -->|查询增量| L0
    L0 -->|增量数据| SS
    SS -->|合并结果| R
```

## 写入流程

```mermaid
graph LR
    C["客户端<br/>INSERT/UPDATE"]
    SS["SqlServer"]
    L0["TransServer<br/>L0层"]
    WAL["预写日志<br/>WAL"]
    M["MemTable"]
    R["确认"]

    C -->|SQL更新| SS
    SS -->|写入| L0
    L0 -->|记录| WAL
    L0 -->|缓存| M
    L0 -->|返回| R
```

## 关键接口契约

### TenantContext 接口
- `getTenantId()` - 获取租户ID
- `getCpuQuota()` - 获取CPU配额
- `getMemoryQuota()` - 获取内存配额
- `getDiskQuota()` - 获取磁盘配额

### ResourceStats 接口
- `getCpuUsage()` - CPU使用统计
- `getMemoryUsage()` - 内存使用统计
- `getDiskUsage()` - 磁盘使用统计

### CpuResourceManager 接口
- `allocateCpu(tenantId, cores)` - 分配CPU
- `releaseCpu(tenantId)` - 释放CPU
- `getCpuStatus(tenantId)` - 获取CPU状态

### MemoryResourceManager 接口
- `allocateMemory(tenantId, bytes)` - 分配内存
- `releaseMemory(tenantId)` - 释放内存
- `getMemoryStatus(tenantId)` - 获取内存状态

### DiskResourceManager 接口
- `allocateDisk(tenantId, bytes)` - 分配磁盘
- `releaseDisk(tenantId)` - 释放磁盘
- `getDiskStatus(tenantId)` - 获取磁盘状态

## 性能指标和SLA

### 目标性能指标
- **请求延迟**: < 10ms (p99)
- **CPU隔离准确度**: > 99.9%
- **内存隔离准确度**: > 99.5%
- **系统开销**: < 5%
- **并发租户数**: 1000+

### 失败模式和容错

| 故障类型 | 行为 | 恢复 |
|---------|------|------|
| CPU配额超限 | 限流/降优先级 | 自动恢复 |
| 内存溢出 | 拒绝新请求 | 等待内存释放 |
| 磁盘满 | 禁止写入 | 数据合并释放空间 |
| cgroup失败 | 降级到基础模式 | 使用线程池比例控制 |
| 租户认证失败 | 拒绝连接 | 客户端重试 |

## 架构设计原则

1. **最小侵入** - 尽量减少对现有架构的修改
2. **渐进式** - 先实现基础隔离，后续可扩展高级功能
3. **租户感知** - 在关键路径传递租户上下文
4. **可配置** - 提供灵活的配置选项
5. **可监控** - 提供完整的资源使用监控能力

---

Generated with GitHub Copilot - HLBPA Mode
