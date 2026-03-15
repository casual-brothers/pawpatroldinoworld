// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PDW : ModuleRules
{
    public PDW(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        string PlatformName = Target.Platform.ToString();

        //OptimizeCode = CodeOptimization.InShippingBuildsOnly;

        PublicDefinitions.Add("PLATFORM_WINDOWS=" + (PlatformName == "Win64" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_WINGDK=" + (PlatformName == "WinGDK" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_XBOXONE=" + ((PlatformName == "XboxOne" || PlatformName == "XboxOneGDK") ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_XSX=" + (PlatformName == "XSX" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_PS4=" + (PlatformName == "PS4" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_PS5=" + (PlatformName == "PS5" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_SWITCH=" + (PlatformName == "Switch" ? "1" : "0"));
        PublicDefinitions.Add("PLATFORM_SWITCH2=" + (PlatformName == "Switch2" ? "1" : "0"));

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

		if (Target.Platform == UnrealTargetPlatform.Switch2)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"StreamPlay"
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
