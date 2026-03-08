Reg delete "HKEY_CURRENT_USER\SOFTWARE\Epic Games\Unreal Engine\Builds" /v 5.6-NebulaEngine-PDW /f
RegisterEngineVersion.cmd 5.6-NebulaEngine-PDW
call ..\SetPignore.bat