p4 set P4PORT=192.168.51.5:1666 
p4 set P4USER=%1
p4 set P4CLIENT=%2

cd /D %~dp0..
p4 edit Binaries\...
p4 edit Plugins\...\Binaries\...
p4 edit Plugins\LowEntryExtStdLib\Intermediate\Build\Win64\UE4Editor\Development\LowEntryExtendedStandardLibrary\UE4Editor-LowEntryExtendedStandardLibrary.lib
p4 edit Plugins\LowEntryExtStdLib\Intermediate\Build\Win64\UE4Editor\Development\LowEntryExtendedStandardLibraryEditor\UE4Editor-LowEntryExtendedStandardLibraryEditor.lib
p4 edit Plugins\LowEntryExtStdLib\Intermediate\Build\Win64\UE4Editor\Inc\...
p4 edit Plugins\LowEntryExtStdLib\Intermediate\Build\Win64\UE4\...

cd ..
p4 edit NebulaEngine_Retail\Engine\Binaries\DotNET\DotNETUtilities.*
p4 edit NebulaEngine_Retail\Engine\Binaries\DotNET\UnrealBuildTool.*
p4 edit NebulaEngine_Retail\Engine\Intermediate\Build\LastBuiltTargets.txt
p4 edit NebulaEngine_Retail\Engine\Programs\UnrealBuildTool\...
p4 edit NebulaEngine_Retail\Engine\Intermediate\Build\BuildRules\...
p4 edit NebulaEngine_Retail\Engine\Source\Programs\DotNETCommon\DotNETUtilities\obj\Development\DotNETUtilities.csproj.FileListAbsolute.txt
p4 edit NebulaEngine_Retail\Engine\Source\Programs\DotNETCommon\DotNETUtilities\obj\Development\DotNETUtilities.csprojResolveAssemblyReference.cache
p4 edit NebulaEngine_Retail\Engine\Source\Programs\UnrealBuildTool\obj\Development\UnrealBuildTool.csproj.FileListAbsolute.txt
p4 edit NebulaEngine_Retail\Engine\Source\Programs\UnrealBuildTool\obj\Development\UnrealBuildTool.csprojResolveAssemblyReference.cache
p4 edit NebulaEngine_Retail\Engine\Source\Programs\AutomationToolLauncher\obj\...
p4 edit NebulaEngine_Retail\Engine\Intermediate\Build\Win64\UE4Editor\Development\*.res

%~dp0..\..\NebulaEngine_Retail\Engine\Binaries\DotNet\AutomationTool\UnrealBuildTool.exe -projectfiles -project=%~dp0..\PDW.uproject -game -rocket -progress
