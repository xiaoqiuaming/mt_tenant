#pragma once

#include <string>
#include <unordered_map>

namespace yao {

/**
 * @brief 配置管理类
 * 负责系统配置的读取和管理
 */
class ConfigManager {
public:
    /**
     * @brief 获取单例实例
     * @return ConfigManager实例
     */
    static ConfigManager& getInstance();

    /**
     * @brief 加载配置文件
     * @param configFile 配置文件路径
     * @return 是否加载成功
     */
    bool loadConfig(const std::string& configFile);

    /**
     * @brief 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;

    /**
     * @brief 获取整数配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    int getInt(const std::string& key, int defaultValue = 0) const;

    /**
     * @brief 获取布尔配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;

    /**
     * @brief 设置配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * @brief 设置整数配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief 设置布尔配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setBool(const std::string& key, bool value);

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    std::unordered_map<std::string, std::string> m_config;  ///< 配置映射
};

} // namespace yao