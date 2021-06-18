// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class un1verse_ntwrk_dmoEditorTarget : TargetRules
{
	public un1verse_ntwrk_dmoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("un1verse_ntwrk_dmo");
	}
}
