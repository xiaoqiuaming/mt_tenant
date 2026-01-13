cmake_minimum_required(VERSION 3.10)
project(YaoBaseTenant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(src)

# 库源文件 (不包含main.cpp)
set(LIB_SOURCES
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

# 创建库
add_library(yaobase_lib STATIC ${LIB_SOURCES})

# 可执行文件
add_executable(yaobase_tenant src/main.cpp)
target_link_libraries(yaobase_tenant yaobase_lib)

# MinGW/GCC 在 C++17 下的 std::filesystem 需要显式链接 stdc++fs
if (MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(yaobase_tenant stdc++fs)
    target_link_libraries(yaobase_lib stdc++fs)
endif()

# GoogleTest 配置
option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # 下载并配置 GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    
    # 在Windows下，防止覆盖父项目的编译器/链接器设置
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # 包含测试子目录
    add_subdirectory(tests)
endif()