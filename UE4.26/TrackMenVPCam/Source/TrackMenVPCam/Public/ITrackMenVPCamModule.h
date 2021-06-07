/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

namespace TrackMen {

	static const char* ModuleName = (const char*)"TrackMenVPCam";
	/**
	* Defines the main plugin module.
	*/
	class ITrackMenVPCamModule : public IModuleInterface {
	public:

		static inline ITrackMenVPCamModule& Get() {
			return FModuleManager::LoadModuleChecked< ITrackMenVPCamModule >(ModuleName);
		}

		static inline bool IsAvailable() {
			return FModuleManager::Get().IsModuleLoaded(ModuleName);
		}
	};

}