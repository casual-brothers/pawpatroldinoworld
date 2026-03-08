// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class PDWEditor : ModuleRules
{
	public PDWEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"PDW",
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Chaos",
			"ChaosVehicles",
			"PhysicsCore",
			"GameplayTags",
			"LevelSequence",
			"ImGui",
			"Slate",
			"SlateCore",
			"NebulaFlow",
			"NebulaGameplay",
			"NebulaGraphics",
			"NebulaTool",
			"MoviePlayer",
			"MovieScene",
			"GameplayAbilities",
			"GameplayTasks",
            "FMODStudio",
			"Blutility",
			"UMG",
			"UMGEditor",
			"EditorScriptingUtilities",
			"UnrealEd",
			"Flow",
			"FlowQuest"
        });
	}
}
