@echo off

if "%1"=="" goto noparam


:build

%~dp0..\..\NebulaEngine_Retail\Engine\Build\BatchFiles\Build.bat -projectfiles  -project=%~dp0..\%1.uproject -game -rocket -progress -log="%~dp0..\Saved/Logs/UnrealVersionSelector.log"

goto done

:noparam
echo Missing Build Params

:done
pause
