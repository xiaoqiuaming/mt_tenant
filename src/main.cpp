#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

// 包含所有头文件
#include "core/tenant/TenantContext.h"
#include "core/tenant/TenantManager.h"
#include "core/resource/BasicResourceStats.h"
#include "core/resource/CpuResourceManager.h"
#include "core/resource/ThreadPoolManager.h"
#include "common/config/ConfigManager.h"
#include "common/utils/Exceptions.h"
#include "common/utils/RequestContext.h"
#include "server/sql/SqlServer.h"
#include "server/sql/ConnectionManager.h"
#include "server/data/DataServer.h"
#include "server/trans/TransServer.h"
#include "server/admin/AdminServer.h"

int main(int argc, char* argv[]) {
    using namespace yao;

    bool runTests = (argc > 1 && std::string(argv[1]) == "--test");
    bool runBenchmark = (argc > 1 && std::string(argv[1]) == "--benchmark");

    if (runTests) {
        return runUnitTests();
    }

    if (runBenchmark) {
        return runBenchmarkTests();
    }

    std::cout << "YaoBase Multi-tenant Resource Isolation Demo" << std::endl;

    // 初始化配置管理器
    auto& config = ConfigManager::getInstance();
    if (!config.loadConfig("config.ini")) {
        std::cout << "Failed to load config, using defaults" << std::endl;
        // 设置默认配置
        config.setBool("enable_cgroup", false);
        config.setInt("total_threads", 120);
    }

    // 创建租户管理器并添加示例租户
    auto& tenantManager = TenantManager::getInstance();
    tenantManager.createTenant("tenant1", 2, 8LL * 1024 * 1024 * 1024, 128LL * 1024 * 1024 * 1024);  // 2 CPU, 8GB, 128GB
    tenantManager.createTenant("tenant2", 1, 4LL * 1024 * 1024 * 1024, 64LL * 1024 * 1024 * 1024);   // 1 CPU, 4GB, 64GB

    // 初始化CPU资源管理器
    auto& cpuManager = CpuResourceManager::getInstance();
    bool enableCgroup = config.getBool("enable_cgroup", false);
    cpuManager.initializeCgroup(enableCgroup);

    // 初始化内存资源管理器
    auto& memoryManager = MemoryResourceManager::getInstance();
    size_t totalMemoryMB = config.getInt("total_memory_mb", 8192);
    memoryManager.initialize(totalMemoryMB);

    // 初始化磁盘资源管理器
    auto& diskManager = DiskResourceManager::getInstance();
    size_t totalDiskGB = config.getInt("total_disk_gb", 100);
    diskManager.initialize(totalDiskGB);

    // 启动CPU监控
    auto& cpuMonitor = CpuMonitor::getInstance();
    int monitoringInterval = config.getInt("monitoring_interval_ms", 2000);
    cpuMonitor.startMonitoring(monitoringInterval);

    // 初始化线程池管理器
    auto& threadManager = ThreadPoolManager::getInstance();
    size_t totalThreads = config.getInt("total_threads", 120);
    if (!threadManager.initialize(totalThreads, enableCgroup)) {
        std::cout << "Failed to initialize ThreadPoolManager" << std::endl;
        return 1;
    }

    // 创建服务器实例
    auto sqlServer = std::make_unique<YaoSqlServer>();
    auto dataServer = std::make_unique<YaoDataServer>();
    auto transServer = std::make_unique<YaoTransServer>();
    auto adminServer = std::make_unique<YaoAdminServer>();

    // 初始化服务器
    if (!sqlServer->initialize() || !dataServer->initialize() ||
        !transServer->initialize() || !adminServer->initialize()) {
        std::cout << "Failed to initialize servers" << std::endl;
        return 1;
    }

    // 启动服务器
    if (!sqlServer->start() || !dataServer->start() ||
        !transServer->start() || !adminServer->start()) {
        std::cout << "Failed to start servers" << std::endl;
        return 1;
    }

    // 模拟多租户并发请求
    std::cout << "Simulating multi-tenant requests..." << std::endl;

    // 模拟tenant1的请求
    auto tenant1 = tenantManager.getTenant("tenant1");
    if (tenant1) {
        auto stats1 = std::make_unique<BasicResourceStats>();
        RequestContext context1(tenant1, std::move(stats1));

        std::cout << "Processing requests for tenant1..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            sqlServer->handleRequest(context1);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // 模拟tenant2的请求
    auto tenant2 = tenantManager.getTenant("tenant2");
    if (tenant2) {
        auto stats2 = std::make_unique<BasicResourceStats>();
        RequestContext context2(tenant2, std::move(stats2));

        std::cout << "Processing requests for tenant2..." << std::endl;
        for (int i = 0; i < 3; ++i) {
            sqlServer->handleRequest(context2);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    }

    // 显示线程池统计信息
    auto sysInfo = threadManager.getSystemThreadInfo();
    std::cout << "System Thread Info:" << std::endl;
    std::cout << "  Total threads: " << sysInfo.totalThreads << std::endl;
    std::cout << "  Allocated threads: " << sysInfo.allocatedThreads << std::endl;
    std::cout << "  System threads: " << sysInfo.systemThreads << std::endl;

    auto tenant1Info = threadManager.getTenantThreadInfo("tenant1");
    std::cout << "Tenant1 Thread Info:" << std::endl;
    std::cout << "  Total threads: " << tenant1Info.totalThreads << std::endl;
    std::cout << "  Busy threads: " << tenant1Info.busyThreads << std::endl;
    std::cout << "  Queue size: " << tenant1Info.queueSize << std::endl;

    auto tenant2Info = threadManager.getTenantThreadInfo("tenant2");
    std::cout << "Tenant2 Thread Info:" << std::endl;
    std::cout << "  Total threads: " << tenant2Info.totalThreads << std::endl;
    std::cout << "  Busy threads: " << tenant2Info.busyThreads << std::endl;
    std::cout << "  Queue size: " << tenant2Info.queueSize << std::endl;

    // 等待一段时间让任务完成
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 停止服务器
    sqlServer->stop();
    dataServer->stop();
    transServer->stop();
    adminServer->stop();

    // 停止监控
    cpuMonitor.stopMonitoring();

    // 清理线程池
    threadManager.shutdown();

    std::cout << "Demo completed successfully" << std::endl;
    return 0;
}

int runUnitTests() {
    using namespace yao;

    std::cout << "Running unit tests..." << std::endl;

    // 测试租户管理器
    auto& tm = TenantManager::getInstance();
    bool test1 = tm.createTenant("test_tenant", 1, 1024 * 1024 * 1024, 10 * 1024 * 1024 * 1024);
    auto tenant = tm.getTenant("test_tenant");
    bool test2 = (tenant != nullptr && tenant->getTenantId() == "test_tenant");
    bool test3 = tm.removeTenant("test_tenant");
    bool test4 = (tm.getTenant("test_tenant") == nullptr);

    std::cout << "TenantManager tests: " << (test1 && test2 && test3 && test4 ? "PASSED" : "FAILED") << std::endl;

    // 测试认证器
    tm.createTenant("auth_test", 1, 1024 * 1024 * 1024, 10 * 1024 * 1024 * 1024);
    TenantAuthenticator auth;
    std::string authResult = auth.authenticate("user@auth_test", "password");
    bool test5 = (authResult == "auth_test");
    std::string authFail = auth.authenticate("user@nonexistent", "password");
    bool test6 = (authFail == "");
    tm.removeTenant("auth_test");

    std::cout << "TenantAuthenticator tests: " << (test5 && test6 ? "PASSED" : "FAILED") << std::endl;

    // 测试CPU配额检查器
    tm.createTenant("quota_test", 50, 1024 * 1024 * 1024, 10 * 1024 * 1024 * 1024);
    CpuQuotaChecker checker;
    bool test7 = checker.checkQuota("quota_test");
    checker.updateUsage("quota_test", 0.6);
    bool test8 = !checker.checkQuota("quota_test");
    tm.removeTenant("quota_test");

    std::cout << "CpuQuotaChecker tests: " << (test7 && test8 ? "PASSED" : "FAILED") << std::endl;

    std::cout << "All tests completed." << std::endl;
    return 0;
}

int runBenchmarkTests() {
    using namespace yao;

    std::cout << "Running benchmark tests..." << std::endl;

    // 初始化配置
    auto& config = ConfigManager::getInstance();
    config.loadConfig("config.ini");

    // 创建租户
    auto& tm = TenantManager::getInstance();
    tm.createTenant("bench_tenant1", 2, 4LL * 1024 * 1024 * 1024, 50LL * 1024 * 1024 * 1024);
    tm.createTenant("bench_tenant2", 1, 2LL * 1024 * 1024 * 1024, 25LL * 1024 * 1024 * 1024);

    // 初始化管理器
    auto& cpuManager = CpuResourceManager::getInstance();
    cpuManager.initializeCgroup(config.getBool("enable_cgroup", false));

    auto& memoryManager = MemoryResourceManager::getInstance();
    memoryManager.initialize(config.getInt("total_memory_mb", 8192));

    auto& diskManager = DiskResourceManager::getInstance();
    diskManager.initialize(config.getInt("total_disk_gb", 100));

    auto& threadManager = ThreadPoolManager::getInstance();
    threadManager.initialize(config.getInt("total_threads", 120), config.getBool("enable_cgroup", false));

    auto& cpuMonitor = CpuMonitor::getInstance();
    cpuMonitor.startMonitoring(config.getInt("monitoring_interval_ms", 2000));

    // 初始化服务器
    auto sqlServer = std::make_unique<YaoSqlServer>();
    sqlServer->initialize();
    sqlServer->start();

    // 性能测试：模拟高负载
    auto tenant1 = tm.getTenant("bench_tenant1");
    auto tenant2 = tm.getTenant("bench_tenant2");

    const int numRequests = 1000;
    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {  // 10个并发线程
        threads.emplace_back([&]() {
            for (int j = 0; j < numRequests / 10; ++j) {
                auto stats = std::make_unique<BasicResourceStats>();
                RequestContext context(tenant1, std::move(stats));
                sqlServer->handleRequest(context);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));  // 模拟延迟
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Benchmark completed in " << duration.count() << " ms" << std::endl;
    std::cout << "Requests per second: " << (numRequests * 1000.0 / duration.count()) << std::endl;

    // 显示统计
    auto sysInfo = threadManager.getSystemThreadInfo();
    std::cout << "System threads: " << sysInfo.totalThreads << std::endl;
    std::cout << "Allocated threads: " << sysInfo.allocatedThreads << std::endl;

    // 清理
    sqlServer->stop();
    cpuMonitor.stopMonitoring();
    threadManager.shutdown();
    tm.removeTenant("bench_tenant1");
    tm.removeTenant("bench_tenant2");

    std::cout << "Benchmark tests completed." << std::endl;
    return 0;
}