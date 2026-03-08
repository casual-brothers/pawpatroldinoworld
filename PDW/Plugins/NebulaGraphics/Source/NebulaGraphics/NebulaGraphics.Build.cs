using UnrealBuildTool;
using System.IO;

public class NebulaGraphics : ModuleRules
{
	public NebulaGraphics(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });

        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RHI",
                "NebulaTool",
				"NebulaFlow",
				"GameplayTags",
                "FMODStudio",
                "HairStrandsCore",
            }
        );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Niagara",
            }
        );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}