cd /D %~dp0
if exist %~dp0..\Releases\%2\%1 rmdir /Q /S %~dp0..\Releases\%2\%1
if exist %~dp0..\Saved\StagedBuilds\%1 rmdir /Q /S %~dp0..\Saved\StagedBuilds\%1
