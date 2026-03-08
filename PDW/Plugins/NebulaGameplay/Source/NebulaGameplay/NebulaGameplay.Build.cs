// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NebulaGameplay : ModuleRules
{
	public NebulaGameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
				"CinematicCamera",
                "PhysicsCore",
                "EnhancedInput"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "UMG",
                "MovieScene",
                "Media",
                "MediaAssets"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}
