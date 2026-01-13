#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

namespace yao {

/**
 * @brief cgroup控制器
 * 管理Linux cgroup v1的CPU子系统
 */
class CgroupController {
public:
    CgroupController(const std::string& basePath = "/sys/fs/cgroup/cpu/yaobase");
    ~CgroupController();

    /**
     * @brief 初始化cgroup
     * @return 是否成功
     */
    bool initialize();

    /**
     * @brief 创建租户cgroup
     * @param tenantId 租户ID
     * @param cpuShares CPU份额 (默认1024)
     * @return 是否成功
     */
    bool createTenantCgroup(const std::string& tenantId, int cpuShares = 1024);

    /**
     * @brief 删除租户cgroup
     * @param tenantId 租户ID
     * @return 是否成功
     */
    bool removeTenantCgroup(const std::string& tenantId);

    /**
     * @brief 设置CPU份额
     * @param tenantId 租户ID
     * @param shares CPU份额
     * @return 是否成功
     */
    bool setCpuShares(const std::string& tenantId, int shares);

    /**
     * @brief 添加线程到cgroup
     * @param tenantId 租户ID
     * @param threadId 线程ID
     * @return 是否成功
     */
    bool addThread(const std::string& tenantId, std::thread::id threadId);

    /**
     * @brief 从cgroup移除线程
     * @param tenantId 租户ID
     * @param threadId 线程ID
     * @return 是否成功
     */
    bool removeThread(const std::string& tenantId, std::thread::id threadId);

    /**
     * @brief 获取CPU使用统计
     * @param tenantId 租户ID
     * @return CPU使用时间（纳秒）
     */
    uint64_t getCpuUsage(const std::string& tenantId) const;

    /**
     * @brief 获取限制统计
     * @param tenantId 租户ID
     * @return 被限制的时间（纳秒）
     */
    uint64_t getThrottledTime(const std::string& tenantId) const;

private:
    /**
     * @brief 写入cgroup文件
     */
    bool writeCgroupFile(const std::string& path, const std::string& content) const;

    /**
     * @brief 读取cgroup文件
     */
    std::string readCgroupFile(const std::string& path) const;

    std::string basePath_;
    std::unordered_map<std::string, std::vector<std::thread::id>> tenantThreads_;
    mutable std::mutex mutex_;
};

} // namespace yao