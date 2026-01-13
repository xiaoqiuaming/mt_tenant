@echo off
REM 设置mingw64工具链路径
set PATH=D:\msys64\mingw64\bin;D:\msys64\usr\bin;%PATH%

REM 进入项目目录
cd /d "D:\工作\tenant\copilot_tenant_demo"

REM 检查build目录
if not exist build mkdir build
cd build

REM 运行CMake配置
echo.
echo ========== CMake 配置 ==========
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++.exe -DCMAKE_C_COMPILER=gcc.exe ..

if errorlevel 1 (
    echo CMake 配置失败！
    pause
    exit /b 1
)

REM 编译
echo.
echo ========== 编译中 ==========
mingw32-make -j4

if errorlevel 1 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo ========== 编译成功！ ==========
echo 可执行文件: %CD%\yaobase_tenant.exe
echo.
echo 运行方式:
echo   演示:    yaobase_tenant.exe
echo   单元测试: yaobase_tenant.exe --test
echo   性能测试: yaobase_tenant.exe --benchmark
echo.
pause
