// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPIpvmulti : ModuleRules
{
	public FPIpvmulti(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"UMG",

			// Online / Sessions
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});

		// Steam subsystem is usually dynamically loaded
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}