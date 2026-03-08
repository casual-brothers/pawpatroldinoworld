// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class NebulaFlow_UncookedOnly : ModuleRules
{
    public NebulaFlow_UncookedOnly(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
                "NebulaFlow"
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "KismetCompiler",
                "Kismet",
                "KismetWidgets",
                "PropertyEditor",
                "UnrealEd",
                "BlueprintGraph",
                "ToolMenus",
                "GameplayTags"
            }
        );
    }
}