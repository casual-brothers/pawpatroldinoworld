cd /D %~dp0..
cd ..
cd NebulaEngine_Retail\Engine\Binaries\Win64
rem AutomationTool Localise -UEProjectRoot="%1" -UEProjectDirectory="" -UEProjectName="%2" -LocalizationProjectNames="Game" -LocalizationSteps="Compile" -LocalizationProvider=""
UnrealEditor.exe %1 -run=GatherText -config=Config\Localization\Game_Compile.ini
