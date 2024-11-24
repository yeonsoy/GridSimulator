// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GridSimulator : ModuleRules
{
	public GridSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
        PublicDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PublicDependencyModuleNames.AddRange(new string[] { "HTTP", "Json", "JsonUtilities" });
    }
}
