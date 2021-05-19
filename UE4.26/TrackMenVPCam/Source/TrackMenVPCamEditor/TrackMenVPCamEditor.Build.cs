/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

namespace UnrealBuildTool.Rules
{
    public class TrackMenVPCamEditor : ModuleRules
    {
        public TrackMenVPCamEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "SlateCore",
                    "Slate",
                    "LiveLinkInterface",
                    "TrackMenVPCam"
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "InputCore",
                }
            );
        }
    }
}
