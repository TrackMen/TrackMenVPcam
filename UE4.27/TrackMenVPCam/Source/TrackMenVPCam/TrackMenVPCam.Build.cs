/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

namespace UnrealBuildTool.Rules
{
    public class TrackMenVPCam : ModuleRules
	{
		public TrackMenVPCam(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "CoreUObject",
                    "Engine",
                    "LiveLink",
                    "LiveLinkComponents",
                    "LiveLinkInterface",
                    "Projects",
                    "CinematicCamera",
                    "Networking",
                    "Sockets"
				}
			);
        }
	}
}
