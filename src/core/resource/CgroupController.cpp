#include "CgroupController.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace yao {

CgroupController::CgroupController(const std::string& basePath)
    : basePath_(basePath) {
}

CgroupController::~CgroupController() {
    // 清理所有租户cgroup
    for (const auto& [tenantId, _] : tenantThreads_) {
        removeTenantCgroup(tenantId);
    }
}

bool CgroupController::initialize() {
    try {
        // 创建基础目录
        if (!fs::exists(basePath_)) {
            fs::create_directories(basePath_);
        }

        // 设置目录权限
        fs::permissions(basePath_, fs::perms::owner_all | fs::perms::group_read | fs::perms::group_execute);

        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to initialize cgroup: " << e.what() << std::endl;
        return false;
    }
}

bool CgroupController::createTenantCgroup(const std::string& tenantId, int cpuShares) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string tenantPath = basePath_ + "/" + tenantId;

    try {
        // 创建租户目录
        if (!fs::exists(tenantPath)) {
            fs::create_directory(tenantPath);
        }

        // 设置CPU份额
        if (!writeCgroupFile(tenantPath + "/cpu.shares", std::to_string(cpuShares))) {
            return false;
        }

        // 初始化线程列表
        tenantThreads_[tenantId] = std::vector<std::thread::id>();

        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to create tenant cgroup " << tenantId << ": " << e.what() << std::endl;
        return false;
    }
}

bool CgroupController::removeTenantCgroup(const std::string& tenantId) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string tenantPath = basePath_ + "/" + tenantId;

    try {
        // 移除目录
        if (fs::exists(tenantPath)) {
            fs::remove_all(tenantPath);
        }

        // 清理线程列表
        tenantThreads_.erase(tenantId);

        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to remove tenant cgroup " << tenantId << ": " << e.what() << std::endl;
        return false;
    }
}

bool CgroupController::setCpuShares(const std::string& tenantId, int shares) {
    std::string tenantPath = basePath_ + "/" + tenantId;
    return writeCgroupFile(tenantPath + "/cpu.shares", std::to_string(shares));
}

bool CgroupController::addThread(const std::string& tenantId, std::thread::id threadId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = tenantThreads_.find(tenantId);
    if (it == tenantThreads_.end()) {
        return false;
    }

    // 检查是否已存在
    auto& threads = it->second;
    if (std::find(threads.begin(), threads.end(), threadId) != threads.end()) {
        return true; // 已存在
    }

    // 添加到cgroup
    std::string tenantPath = basePath_ + "/" + tenantId;
    std::stringstream ss;
    ss << threadId;
    if (!writeCgroupFile(tenantPath + "/tasks", ss.str())) {
        return false;
    }

    threads.push_back(threadId);
    return true;
}

bool CgroupController::removeThread(const std::string& tenantId, std::thread::id threadId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = tenantThreads_.find(tenantId);
    if (it == tenantThreads_.end()) {
        return false;
    }

    auto& threads = it->second;
    auto threadIt = std::find(threads.begin(), threads.end(), threadId);
    if (threadIt == threads.end()) {
        return true; // 不存在
    }

    threads.erase(threadIt);
    return true;
}

uint64_t CgroupController::getCpuUsage(const std::string& tenantId) const {
    std::string tenantPath = basePath_ + "/" + tenantId;
    std::string content = readCgroupFile(tenantPath + "/cpuacct.usage");
    if (content.empty()) return 0;

    try {
        return std::stoull(content);
    } catch (const std::exception&) {
        return 0;
    }
}

uint64_t CgroupController::getThrottledTime(const std::string& tenantId) const {
    std::string tenantPath = basePath_ + "/" + tenantId;
    std::string content = readCgroupFile(tenantPath + "/cpu.stat");

    // 解析throttled_time
    size_t pos = content.find("throttled_time");
    if (pos == std::string::npos) return 0;

    std::stringstream ss(content.substr(pos));
    std::string line;
    while (std::getline(ss, line)) {
        if (line.find("throttled_time") != std::string::npos) {
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                try {
                    return std::stoull(line.substr(equalPos + 1));
                } catch (const std::exception&) {
                    return 0;
                }
            }
        }
    }

    return 0;
}

bool CgroupController::writeCgroupFile(const std::string& path, const std::string& content) const {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content << std::endl;
        return file.good();
    } catch (const std::exception& e) {
        std::cerr << "Failed to write cgroup file " << path << ": " << e.what() << std::endl;
        return false;
    }
}

std::string CgroupController::readCgroupFile(const std::string& path) const {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return "";
        }
        std::string content;
        std::getline(file, content);
        return content;
    } catch (const std::exception&) {
        return "";
    }
}

} // namespace yao