// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class un1verse_ntwrk_dmoTarget : TargetRules
{
	public un1verse_ntwrk_dmoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("un1verse_ntwrk_dmo");
	}
}
