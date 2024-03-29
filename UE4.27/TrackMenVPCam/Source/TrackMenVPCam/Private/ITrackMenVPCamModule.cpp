/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#include "ITrackMenVPCamModule.h"
#include "CoreMinimal.h"
#include "PluginLogging.h"


class FTrackMenVPCamModule : public TrackMen::ITrackMenVPCamModule {
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FTrackMenVPCamModule, TrackMenVPCam)

void FTrackMenVPCamModule::StartupModule() {
	UE_LOG(LogTrackMenPlugin, Display, TEXT("TrackMen: StartupModule"));
}

void FTrackMenVPCamModule::ShutdownModule() {
	UE_LOG(LogTrackMenPlugin, Display, TEXT("TrackMen: ShutdownModule"));
}
