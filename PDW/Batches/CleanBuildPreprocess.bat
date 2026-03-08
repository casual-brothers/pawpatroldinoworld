@echo off
p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=%1
p4 set P4CLIENT=%2

cd /D %~dp0..\Source\PDW\Public
p4 revert BuildPreprocess.h
REM cd /D %~dp0..\Plugins\GameAnalytics\Source\GameAnalytics\Public
REM p4 revert BuildPreprocess.h