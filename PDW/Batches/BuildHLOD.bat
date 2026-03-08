@echo on

cd /D %~dp0..
cd Content\__ExternalActors__

attrib *.* -r /s

if "%1"=="" goto noparam

%~dp0..\..\NebulaEngine_Retail\Engine\Binaries\Win64\UnrealEditor-Cmd.exe "D:\PRJ\%1\Main\%1.uproject" -run=WorldPartitionBuilderCommandlet -AllowCommandletRendering -Builder=WorldPartitionHLODsBuilder -SetupHLODs %2

if not %ERRORLEVEL% ==0 (
	call RevertMain.bat %3 %4
    exit /b %ERRORLEVEL%
) 

%~dp0..\..\NebulaEngine_Retail\Engine\Binaries\Win64\UnrealEditor-Cmd.exe "D:\PRJ\%1\Main\%1.uproject" -run=WorldPartitionBuilderCommandlet -AllowCommandletRendering -Builder=WorldPartitionHLODsBuilder -BuildHLODs %2

if not %ERRORLEVEL% ==0 (
	call RevertMain.bat %3 %4
    exit /b %ERRORLEVEL%
) 

goto done

:noparam
echo Missing Build Params

:done
pause
