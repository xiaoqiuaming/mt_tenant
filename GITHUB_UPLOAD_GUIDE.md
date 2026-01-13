# YaoBase GitHub 上传指南

项目已初始化为本地 Git 仓库。要上传到 GitHub，请按以下步骤操作：

## 1. 在 GitHub 上创建新仓库

访问 https://github.com/new，创建一个新仓库：
- **Repository name**: copilot-tenant-demo 或 yaobase-multi-tenant
- **Description**: YaoBase multi-tenant resource isolation system
- **Public/Private**: 选择你的偏好
- **不要** 初始化 README、.gitignore 或 license（我们已有这些）

## 2. 添加远程仓库

在项目根目录执行：

```bash
# 使用 HTTPS（推荐用于第一次设置）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 或使用 SSH（需要配置 SSH 密钥）
git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git
```

将 `YOUR_USERNAME` 和 `YOUR_REPO_NAME` 替换为你的 GitHub 用户名和仓库名。

## 3. 验证远程仓库

```bash
git remote -v
```

应该显示：
```
origin  https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git (fetch)
origin  https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git (push)
```

## 4. 上传到 GitHub

```bash
git push -u origin master
```

如果是 main 分支：
```bash
git branch -M main
git push -u origin main
```

## 5. 后续提交

```bash
git add .
git commit -m "描述你的更改"
git push
```

## 常用分支命令

```bash
# 查看所有分支
git branch -a

# 创建新分支
git branch feature/your-feature-name

# 切换分支
git checkout feature/your-feature-name

# 创建并切换到新分支
git checkout -b feature/your-feature-name
```

## 项目文件说明

- **ARCHITECTURE.md**: 系统架构设计文档
- **PRODUCT.md**: 产品需求文档
- **CONTRIBUTING.md**: 开发贡献指南
- **README.md**: 项目概述和使用说明
- **config.ini**: 配置文件示例
- **CMakeLists.txt**: CMake 构建配置
- **.gitignore**: Git 忽略配置

## 当前提交信息

```
Initial commit: YaoBase multi-tenant resource isolation system with CPU, memory, and disk quota management
```

包含 58 个文件，总计 4368 行代码。

## 需要帮助？

- Git 官方文档: https://git-scm.com/doc
- GitHub 帮助: https://docs.github.com
- 问题排查: https://docs.github.com/en/authentication