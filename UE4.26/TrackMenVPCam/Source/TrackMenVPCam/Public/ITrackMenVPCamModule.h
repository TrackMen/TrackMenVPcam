/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
* Defines the main plugin module.
*/
class ITrackMenVPCamModule : public IModuleInterface {
public:
	// WARNING: if you change MyModuleName here, also update it in the .cpp file.
	//          Necessary because of Unreal macro idiosyncrasy.
	static constexpr char* MyModuleName = "TrackMenVPCam";

	static inline ITrackMenVPCamModule& Get(){
		return FModuleManager::LoadModuleChecked< ITrackMenVPCamModule >(MyModuleName);
	}

	static inline bool IsAvailable(){
		return FModuleManager::Get().IsModuleLoaded(MyModuleName);
	}
};

