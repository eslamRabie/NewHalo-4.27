// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NewHalo : ModuleRules
{
	public NewHalo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "SlateCore", "UMG", "NetCore", "OnlineSubsystem" });
	}
}
