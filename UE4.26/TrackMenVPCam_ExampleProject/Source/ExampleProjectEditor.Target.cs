/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

using UnrealBuildTool;
using System.Collections.Generic;

public class ExampleProjectEditorTarget : TargetRules
{
	public ExampleProjectEditorTarget(TargetInfo Target) : base(Target)
	{
        DefaultBuildSettings = BuildSettingsVersion.V2;

        Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "ExampleProject" } );
	}
}
