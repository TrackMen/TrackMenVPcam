/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

using UnrealBuildTool;
using System.Collections.Generic;

public class ExampleProjectTarget : TargetRules
{
	public ExampleProjectTarget(TargetInfo Target) : base(Target)
	{
        DefaultBuildSettings = BuildSettingsVersion.V2;

        Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "ExampleProject" } );
	}
}
