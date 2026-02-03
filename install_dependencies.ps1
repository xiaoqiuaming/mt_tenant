# Install PPTX Skill Dependencies

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  PPTX Skill Dependency Installer" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# Check for admin rights
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Host "ERROR: This script requires administrator privileges" -ForegroundColor Red
    Write-Host "Please run PowerShell as Administrator and try again" -ForegroundColor Yellow
    Start-Sleep -Seconds 2
    exit 1
}

# Step 1: Install Chocolatey
Write-Host ""
Write-Host "[1/3] Checking Chocolatey package manager..." -ForegroundColor Green

$chocoCmd = Get-Command choco -ErrorAction SilentlyContinue
if (-not $chocoCmd) {
    Write-Host "  Installing Chocolatey..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    $url = 'https://community.chocolatey.org/install.ps1'
    Invoke-Expression ((New-Object System.Net.WebClient).DownloadString($url))
    Write-Host "  Chocolatey installed successfully" -ForegroundColor Green
} else {
    Write-Host "  Chocolatey is already installed" -ForegroundColor Green
}

# Step 2: Install Node.js and Python
Write-Host ""
Write-Host "[2/3] Installing Node.js LTS and Python 3..." -ForegroundColor Green

$nodeCmd = Get-Command node -ErrorAction SilentlyContinue
if (-not $nodeCmd) {
    Write-Host "  Installing Node.js LTS..." -ForegroundColor Yellow
    choco install nodejs-lts -y
    Write-Host "  Node.js installed successfully" -ForegroundColor Green
} else {
    Write-Host "  Node.js is already installed" -ForegroundColor Green
}

$pythonCmd = Get-Command python -ErrorAction SilentlyContinue
if (-not $pythonCmd) {
    Write-Host "  Installing Python 3..." -ForegroundColor Yellow
    choco install python -y
    Write-Host "  Python installed successfully" -ForegroundColor Green
} else {
    Write-Host "  Python is already installed" -ForegroundColor Green
}

# Refresh PATH
$env:Path = [System.Environment]::GetEnvironmentVariable('Path', 'Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path', 'User')

# Step 3: Install npm and pip packages
Write-Host ""
Write-Host "[3/3] Installing npm and pip packages..." -ForegroundColor Green

$npmCmd = Get-Command npm -ErrorAction SilentlyContinue
if ($npmCmd) {
    Write-Host "  Installing npm global packages..." -ForegroundColor Yellow
    npm install -g pptxgenjs 2>&1 | Out-Null
    npm install -g playwright 2>&1 | Out-Null
    npm install -g sharp 2>&1 | Out-Null
    npm install -g react-icons react react-dom 2>&1 | Out-Null
    Write-Host "  npm packages installed successfully" -ForegroundColor Green
    
    Write-Host "  Initializing Playwright..." -ForegroundColor Yellow
    npx playwright install chromium 2>&1 | Out-Null
    Write-Host "  Playwright initialized successfully" -ForegroundColor Green
} else {
    Write-Host "  ERROR: npm not found" -ForegroundColor Red
}

$pipCmd = Get-Command pip -ErrorAction SilentlyContinue
if ($pipCmd) {
    Write-Host "  Upgrading pip..." -ForegroundColor Yellow
    python -m pip install --upgrade pip 2>&1 | Out-Null
    
    Write-Host "  Installing Python packages..." -ForegroundColor Yellow
    pip install 'markitdown[pptx]' 2>&1 | Out-Null
    pip install defusedxml 2>&1 | Out-Null
    Write-Host "  Python packages installed successfully" -ForegroundColor Green
} else {
    Write-Host "  ERROR: pip not found" -ForegroundColor Red
}

# Verify installation
Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "  Installation Complete!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""

if (Get-Command node -ErrorAction SilentlyContinue) {
    $v = node --version
    Write-Host "OK Node.js: $v" -ForegroundColor Green
}

if (Get-Command npm -ErrorAction SilentlyContinue) {
    $v = npm --version
    Write-Host "OK npm: $v" -ForegroundColor Green
}

if (Get-Command python -ErrorAction SilentlyContinue) {
    $v = python --version 2>&1
    Write-Host "OK Python: $v" -ForegroundColor Green
}

if (Get-Command pip -ErrorAction SilentlyContinue) {
    $v = pip --version 2>&1
    Write-Host "OK pip: $v" -ForegroundColor Green
}

Write-Host ""
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "IMPORTANT NEXT STEPS:" -ForegroundColor Yellow
Write-Host "1. Close all PowerShell windows" -ForegroundColor White
Write-Host "2. Close and restart VS Code" -ForegroundColor White
Write-Host "3. Run: node --version" -ForegroundColor White
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Press Enter to exit..." -ForegroundColor White
Read-Host
