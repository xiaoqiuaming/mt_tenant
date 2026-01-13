# YaoBase 多租户项目编译脚本

# 添加mingw64工具链到PATH
$env:PATH = "D:\msys64\mingw64\bin;D:\msys64\usr\bin;" + $env:PATH

# 进入build目录
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$buildDir = Join-Path $projectRoot "build"

if (-not (Test-Path $buildDir)) {
    Write-Host "创建build目录..." -ForegroundColor Green
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Set-Location $buildDir
Write-Host "当前目录: $(Get-Location)" -ForegroundColor Cyan

# 配置CMake
Write-Host "`n配置CMake..." -ForegroundColor Green
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="g++.exe" -DCMAKE_C_COMPILER="gcc.exe" ..

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nCMake配置成功！" -ForegroundColor Green
    
    # 编译
    Write-Host "`n开始编译..." -ForegroundColor Green
    mingw32-make -j4
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✓ 编译成功！" -ForegroundColor Green
        Write-Host "`n可执行文件位置: $(Join-Path $buildDir 'yaobase_tenant.exe')" -ForegroundColor Cyan
        Write-Host "`n运行方式:" -ForegroundColor Yellow
        Write-Host "  - 演示: .\build\yaobase_tenant.exe" -ForegroundColor White
        Write-Host "  - 单元测试: .\build\yaobase_tenant.exe --test" -ForegroundColor White
        Write-Host "  - 性能测试: .\build\yaobase_tenant.exe --benchmark" -ForegroundColor White
    } else {
        Write-Host "`n✗ 编译失败！" -ForegroundColor Red
    }
} else {
    Write-Host "`nCMake配置失败！" -ForegroundColor Red
}
