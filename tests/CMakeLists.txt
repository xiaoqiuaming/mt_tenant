# 测试可执行文件

# 单元测试源文件
set(UNIT_TEST_SOURCES
    unit/TenantContextTest.cpp
    unit/TenantManagerTest.cpp
    unit/TenantAuthenticatorTest.cpp
    unit/CpuResourceManagerTest.cpp
    unit/MemoryResourceManagerTest.cpp
    unit/DiskResourceManagerTest.cpp
    unit/CpuQuotaCheckerTest.cpp
    unit/MemoryQuotaCheckerTest.cpp
    unit/DiskQuotaCheckerTest.cpp
    unit/ConfigManagerTest.cpp
    unit/RequestContextTest.cpp
    unit/BasicResourceStatsTest.cpp
)

# 集成测试源文件
set(INTEGRATION_TEST_SOURCES
    integration/ServerIntegrationTest.cpp
    integration/ResourceIsolationTest.cpp
)

# 创建单元测试可执行文件
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests 
    yaobase_lib
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

if (MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(unit_tests stdc++fs pthread)
endif()

# 创建集成测试可执行文件
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests 
    yaobase_lib
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

if (MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(integration_tests stdc++fs pthread)
endif()

# 添加测试
include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)
