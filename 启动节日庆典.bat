@echo off
chcp 65001 >nul
title 中华华节盛典 · 岁时交互引擎系统
echo 正在启动中华华节盛典全景交互应用...
start "" "msedge.exe" --app="file:///%~dp0index.html" 2>nul || start "" "%~dp0index.html"
exit
