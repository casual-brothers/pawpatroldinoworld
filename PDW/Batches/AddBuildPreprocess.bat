@echo off

p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=%1
p4 set P4CLIENT=%2

setlocal enabledelayedexpansion

REM Estrai la cartella PDW (due livelli sopra %~dp0)
for %%A in ("%~dp0..\..") do set "fullName=%%~nxA"

REM Taglia al primo "_" se presente
for /f "tokens=1 delims=_" %%B in ("!fullName!") do set "projectName=%%B"
echo Nome progetto: !projectName!

cd /D "%~dp0..\Source\!projectName!\Public"

p4 edit BuildPreprocess.h
echo #pragma once> BuildPreprocess.h
echo #define %3 %4>> BuildPreprocess.h
if "%5"=="" goto nextprep
echo #define %5 %6>> BuildPreprocess.h
if "%7"=="" goto nextprep
echo #define %7 %8>> BuildPreprocess.h
if "%9"=="" goto nextprep
echo #define %9 >> BuildPreprocess.h

endlocal

:nextprep
rem cd /D %~dp0..\Plugins\GameFlowPlugin\Source\GameFlowPlugin\Public
rem p4 edit BuildPreprocess.h
rem echo #pragma once> BuildPreprocess.h
rem echo #define %3 %4>> BuildPreprocess.h

rem if "%5"=="" goto nextprep2
rem echo #define %5 %6>> BuildPreprocess.h
rem if "%7"=="" goto nextprep2
rem echo #define %7 %8>> BuildPreprocess.h


:nextprep2
rem cd /D %~dp0..\Plugins\GameAnalytics\Source\GameAnalytics\Public
rem p4 edit BuildPreprocess.h
rem echo #pragma once> BuildPreprocess.h
rem echo #define %3 %4>> BuildPreprocess.h

rem if "%5"=="" goto done
rem echo #define %5 %6>> BuildPreprocess.h
rem if "%7"=="" goto done
rem echo #define %7 %8>> BuildPreprocess.h

:done
