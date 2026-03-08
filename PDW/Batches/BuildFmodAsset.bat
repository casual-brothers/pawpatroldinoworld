cd /D %~dp0..
cd Content\FMOD
attrib *.* -r /s
cd /D %~dp0..
cd ..
cd NebulaEngine_Retail\Engine\Binaries\Win64
UnrealEditor-CMD.exe %1  -run=FMODGenerateAssets -rebuild -unattended -nopause -NoDialog
cd /D %~dp0
