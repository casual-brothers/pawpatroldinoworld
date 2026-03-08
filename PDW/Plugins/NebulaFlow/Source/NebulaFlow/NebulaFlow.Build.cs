// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NebulaFlow : ModuleRules
{
	public NebulaFlow(ReadOnlyTargetRules Target) : base(Target)
	{
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        bAllowConfidentialPlatformDefines = true;

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
