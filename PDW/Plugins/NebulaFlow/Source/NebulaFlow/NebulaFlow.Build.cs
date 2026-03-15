// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NebulaFlow : ModuleRules
{
	public NebulaFlow(ReadOnlyTargetRules Target) : base(Target)
	{
        string PlatformName = Target.Platform.ToString();

        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        bAllowConfidentialPlatformDefines = true;

        PublicDefinitions.Add("PLATFORM_WINDOWS=" + (PlatformName == "Win64" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_WINGDK=" + (PlatformName == "WinGDK" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_XBOXONE=" + ((PlatformName == "XboxOne" || PlatformName == "XboxOneGDK") ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_XSX=" + (PlatformName == "XSX" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_PS4=" + (PlatformName == "PS4" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_PS5=" + (PlatformName == "PS5" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_SWITCH=" + (PlatformName == "Switch" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_SWITCH2=" + (PlatformName == "Switch2" ? "1" : "0"));

        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "FMODStudio",
                "PhysicsCore",
                "DeveloperSettings",
                "OnlineSubsystem",
                "MoviePlayer",
				"ApplicationCore",
				"EnhancedInput",				
				"NebulaGameplay",
				"UMG",
				"CoreOnline",
                "GameplayTags"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "MovieScene",
                "Media",
                "MediaAssets"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}
