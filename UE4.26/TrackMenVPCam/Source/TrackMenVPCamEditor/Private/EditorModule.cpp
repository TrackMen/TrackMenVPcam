/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "EditorModule.h"
#include "CoreMinimal.h"

/**
* Editor module implementation.
*/
class FTrackMenVPCamEditorModule : public TrackMen::EditorModule {
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FTrackMenVPCamEditorModule, TrackMenVPCamEditor)

void FTrackMenVPCamEditorModule::StartupModule() {}

void FTrackMenVPCamEditorModule::ShutdownModule() {}
