@echo off

if "%1"=="" goto noparam


:build

%~dp0..\..\NebulaEngine_Retail\Engine\Build\BatchFiles\RunUAT BuildCookRun -project=%~dp0..\%1.uproject -AdditionalCookerOptions=" -cookprocesscount=3" -encrypt -noP4 -clientconfig=%2 -serverconfig=%2 -nocompile -nocompileeditor -installed -ue4exe=UnrealEditor-Cmd.exe -utf8output -platform=%3 -targetplatform=%3 -build -cook -MapIniSection=AllMaps -unversionedcookedcontent -pak -createreleaseversion=%4 -compressed -stage -package -addcmdline="-SessionId=BFC6B5DB4B77032FF7E3F6B2697AB965 -SessionOwner='Build' -SessionName='New Profile 0'" -archive -archivedirectory="\\192.168.51.4\Builds\%5_%2_%3" %6 %7 %8 %9

goto done

:noparam
echo Missing Build Params
echo BuildProject.bat ProjectName CompilationTarget Platform ReleaseVersion DestinationDir
echo example BuildProject.bat AllStarFruitRacing Shipping Win64 1.0 AFR_%BUILD_TIMESTAMP%_%P4_CHANGELIST% additionalLineCommandParams(fino a 4)

:done
pause
