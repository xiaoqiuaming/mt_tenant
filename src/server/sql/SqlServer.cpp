#include "SqlServer.h"
#include "ConnectionManager.h"
#include "ThreadPoolManager.h"
#include "CpuResourceManager.h"
#include "MemoryResourceManager.h"
#include "MemoryQuotaChecker.h"
#include "ConfigManager.h"
#include "ResourceStats.h"
#include "BasicResourceStats.h"
#include <iostream>
#include <memory>

namespace yao {

YaoSqlServer::YaoSqlServer() : running_(false) {
    connectionManager_ = std::make_unique<ConnectionManager>();
}

YaoSqlServer::~YaoSqlServer() {
    stop();
}

bool YaoSqlServer::handleRequest(const RequestContext& context) {
    if (!running_) {
        std::cerr << "SqlServer is not running" << std::endl;
        return false;
    }

    auto tenant = context.getTenant();
    if (!tenant) {
        std::cerr << "No tenant context in request" << std::endl;
        return false;
    }

    const std::string& tenantId = tenant->getTenantId();

    // 检查CPU资源分配
    auto& cpuManager = CpuResourceManager::getInstance();
    double cpuUsage = cpuManager.getTenantCpuUsage(tenantId);
    if (cpuUsage < 0) {
        // 首次请求，分配CPU资源
        if (!cpuManager.allocateCpuResource(tenant)) {
            std::cerr << "Failed to allocate CPU resource for tenant: " << tenantId << std::endl;
            return false;
        }
    }

    // 检查CPU配额
    if (cpuUsage > 0.8) {  // 80%阈值
        std::cerr << "CPU usage too high for tenant: " << tenantId << " (" << cpuUsage << ")" << std::endl;
        return false;
    }

    // 检查内存资源分配
    auto& memoryManager = MemoryResourceManager::getInstance();
    double memoryUsage = memoryManager.getTenantMemoryUsage(tenantId);
    if (memoryUsage < 0) {
        // 首次请求，分配内存资源
        if (!memoryManager.allocateMemoryResource(tenant)) {
            std::cerr << "Failed to allocate memory resource for tenant: " << tenantId << std::endl;
            return false;
        }
    }

    // 检查内存配额
    auto& memoryChecker = MemoryQuotaChecker::getInstance();
    double requestedMemoryMB = 10.0;  // 示例：请求10MB内存
    if (!memoryChecker.checkQuota(tenant, requestedMemoryMB)) {
        std::cerr << "Memory quota check failed for tenant: " << tenantId << std::endl;
        return false;
    }

    // 创建SQL任务（这里简化，实际应该解析SQL）
    std::string sql = "SELECT * FROM test_table";  // 示例SQL
    auto stats = std::make_unique<BasicResourceStats>();
    auto sqlTask = std::make_unique<SqlTask>(sql, std::make_shared<RequestContext>(context.getTenant(), std::move(stats)));

    // 提交到租户线程池
    auto& threadManager = ThreadPoolManager::getInstance();
    if (!threadManager.submitTask(tenantId, std::move(sqlTask))) {
        std::cerr << "Failed to submit task for tenant: " << tenantId << std::endl;
        return false;
    }

    // 更新CPU使用统计（模拟）
    cpuManager.updateCpuUsage(tenantId, cpuUsage + 0.01);  // 增加1%使用率

    // 更新内存使用统计（模拟）
    double currentMemoryUsage = memoryManager.getTenantMemoryUsage(tenantId);
    if (currentMemoryUsage >= 0) {
        memoryManager.updateMemoryUsage(tenantId, currentMemoryUsage + requestedMemoryMB);
    }

    std::cout << "Request handled for tenant: " << tenantId << std::endl;
    return true;
}

bool YaoSqlServer::initialize() {
    std::cout << "Initializing YaoSqlServer..." << std::endl;

    // 初始化配置管理器
    auto& config = ConfigManager::getInstance();
    bool enableCgroup = config.getBool("enable_cgroup", false);

    // 初始化线程池管理器
    auto& threadManager = ThreadPoolManager::getInstance();
    size_t totalThreads = config.getInt("total_threads", 120);
    if (!threadManager.initialize(totalThreads, enableCgroup)) {
        std::cerr << "Failed to initialize ThreadPoolManager" << std::endl;
        return false;
    }

    // 初始化CPU资源管理器
    auto& cpuManager = CpuResourceManager::getInstance();
    if (!cpuManager.initializeCgroup(enableCgroup)) {
        std::cerr << "Failed to initialize CpuResourceManager" << std::endl;
        return false;
    }

    // 初始化内存资源管理器
    auto& memoryManager = MemoryResourceManager::getInstance();
    size_t totalMemoryMB = config.getInt("total_memory_mb", 8192);  // 默认8GB
    if (!memoryManager.initialize(totalMemoryMB)) {
        std::cerr << "Failed to initialize MemoryResourceManager" << std::endl;
        return false;
    }

    std::cout << "YaoSqlServer initialized successfully" << std::endl;
    return true;
}

bool YaoSqlServer::start() {
    if (running_) {
        return true;
    }

    std::cout << "Starting YaoSqlServer..." << std::endl;

    // TODO: 启动网络监听等

    running_ = true;
    std::cout << "YaoSqlServer started" << std::endl;
    return true;
}

void YaoSqlServer::stop() {
    if (!running_) {
        return;
    }

    std::cout << "Stopping YaoSqlServer..." << std::endl;

    running_ = false;

    // 停止线程池管理器
    auto& threadManager = ThreadPoolManager::getInstance();
    threadManager.shutdown();

    std::cout << "YaoSqlServer stopped" << std::endl;
}

} // namespace yao