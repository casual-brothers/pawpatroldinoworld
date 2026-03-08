cd /D %~dp0..
cd ..
cd NebulaEngine_Retail\Engine\Binaries\Win64
rem AutomationTool Localise -UEProjectRoot="%1" -UEProjectDirectory="" -UEProjectName="%2" -LocalizationProjectNames="Game" -LocalizationSteps="Gather" -LocalizationProvider="" -EnableSCC -DisableSCCSubmit
UnrealEditor.exe %1 -run=GatherText -config=Config\Localization\Game_Gather.ini