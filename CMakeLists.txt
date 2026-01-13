cmake_minimum_required(VERSION 3.10)
project(YaoBaseTenant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(src)

# 源文件
set(SOURCES
    src/main.cpp
    src/core/tenant/TenantContext.cpp
    src/core/tenant/TenantManager.cpp
    src/core/resource/CpuResourceManager.cpp
    src/core/resource/MemoryResourceManager.cpp
    src/core/resource/MemoryQuotaChecker.cpp
    src/core/resource/DiskResourceManager.cpp
    src/core/resource/DiskQuotaChecker.cpp
    src/core/resource/TenantAuthenticator.cpp
    src/core/resource/CpuQuotaChecker.cpp
    src/core/resource/CpuMonitor.cpp
    src/core/resource/LockFreeQueue.cpp
    src/core/resource/TenantThreadGroup.cpp
    src/core/resource/CgroupController.cpp
    src/core/resource/ThreadPoolManager.cpp
    src/core/resource/BasicResourceStats.cpp
    src/common/config/ConfigManager.cpp
    src/common/utils/RequestContext.cpp
    src/server/sql/SqlServer.cpp
    src/server/sql/ConnectionManager.cpp
    src/server/data/DataServer.cpp
    src/server/trans/TransServer.cpp
    src/server/admin/AdminServer.cpp
)

# 可执行文件
add_executable(yaobase_tenant ${SOURCES})

# MinGW/GCC 在 C++17 下的 std::filesystem 需要显式链接 stdc++fs
if (MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(yaobase_tenant stdc++fs)
endif()