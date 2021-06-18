// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class un1verse_ntwrk_dmo : ModuleRules
{
	public un1verse_ntwrk_dmo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
