# Terminal Festival Framework Build Script (PowerShell)
param(
    [string]$Target = "all"
)

$ErrorActionPreference = "Stop"

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host " Terminal Festival Celebration Framework " -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan

$CXX = "g++"
$CXXFLAGS = "-std=c++17 -Wall -Wextra -O2 -I."

if ($Target -eq "all" -or $Target -eq "app") {
    Write-Host "[1/2] Compiling Festival Application (festival_app.exe)..." -ForegroundColor Yellow
    & $CXX $CXXFLAGS.Split(" ") festival/main.cpp -o festival_app.exe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  -> Successfully built festival_app.exe" -ForegroundColor Green
    } else {
        Write-Host "  -> Compilation failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

if ($Target -eq "all" -or $Target -eq "test") {
    Write-Host "[2/2] Compiling Standalone Core Test (core_test.exe)..." -ForegroundColor Yellow
    & $CXX $CXXFLAGS.Split(" ") tests/core_standalone_test.cpp -o core_test.exe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  -> Successfully built core_test.exe" -ForegroundColor Green
    } else {
        Write-Host "  -> Test compilation failed!" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

Write-Host "`nBuild finished successfully!" -ForegroundColor Green
Write-Host "To run the celebration: .\festival_app.exe" -ForegroundColor Cyan
