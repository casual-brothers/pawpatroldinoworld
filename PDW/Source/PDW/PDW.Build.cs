// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PDW : ModuleRules
{
    public PDW(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        //OptimizeCode = CodeOptimization.InShippingBuildsOnly;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "Chaos",
            "PhysicsCore",
            "GameplayTags",
            "LevelSequence",
            "Slate",
            "SlateCore",
            "NebulaFlow",
            "NebulaGameplay",
            "NebulaGraphics",
            "NebulaTool",
            "Niagara",
            "MediaAssets",
            "MoviePlayer",
            "MovieScene",
            "GameplayAbilities",
            "GameplayTasks",
            "FMODStudio",
            "AIModule",
            "PCG",
            "NavigationSystem",
            "ActorSequence",
            "GameplayMessageRuntime",
            "Flow",
            "FlowQuest",
            "SmartObjectsModule",
            "WorldPartitionHelper",
            "NebulaInteractionSystem",
            "ApplicationCore",
            "RHI",
            "Paper2D",
            "BinkMediaPlayer"
        });
		
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

            PublicDependencyModuleNames.AddRange(new string[]
			{
				"ImGui"
			});
		}

        if(Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd",
                "ToxicUtilities"
            });
        }
    }
}
