@echo off
chcp 65001 >nul
title 启动两仪天工 · 智算文心 全栈数字系统
echo ========================================================
echo   ☯ 正在启动 两仪天工 · 智算文心 全栈数字系统...
echo   🏮 华彩岁时盛典 + 📐 John Tromp 拓扑工坊 + ✨ 星汉算筹
echo ========================================================
echo.

where node >nul 2>nul
if %errorlevel% neq 0 (
    echo [提示] 未检测到系统 Node.js 环境，正在以单机轻量客户端模式启动...
    start "" "msedge.exe" --app="file:///%~dp0index.html" 2>nul || start "" "%~dp0index.html"
    exit
)

echo [1/2] 正在启动后端服务 (端口 3000)...
start "中华华节盛典-服务端" /min cmd /c "node server/server.js"

echo [2/2] 等待服务初始化...
timeout /t 2 /nobreak >nul

echo [完成] 正在打开全栈客户端界面...
start "" "msedge.exe" --app="http://localhost:3000" 2>nul || start "" "http://localhost:3000"

echo 盛典全栈服务已就绪！如需关闭服务，请关闭“中华华节盛典-服务端”窗口即可。
exit
