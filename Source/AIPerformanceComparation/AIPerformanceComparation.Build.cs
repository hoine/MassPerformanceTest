// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AIPerformanceComparation : ModuleRules
{
	public AIPerformanceComparation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"MassEntity",
				"MassCommon",
				"MassEntity", 
				"MassSpawner",
				"StructUtils",
				"MassMovement", 
				"MassAIBehavior",
				"StateTreeModule",
				"MassSignals"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"MassNavigation", 
				"NavigationSystem",
				"MassLOD",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
