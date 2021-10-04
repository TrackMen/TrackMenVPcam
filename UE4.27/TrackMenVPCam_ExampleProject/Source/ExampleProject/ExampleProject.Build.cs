/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

using UnrealBuildTool;
using System.IO;

public class ExampleProject : ModuleRules
{
	public ExampleProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore" }
        );


        PrivateDependencyModuleNames.AddRange(new string[] {  });

    }
}
