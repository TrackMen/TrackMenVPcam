/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

namespace TrackMen {

	static const char* EditorModuleName = (const char*)"TrackMenVPCamEditor";
	/**
	* Defines the Editor module.
	*/
	class EditorModule : public IModuleInterface {
	public:

		static inline EditorModule& Get() {
			return FModuleManager::LoadModuleChecked< EditorModule >(EditorModuleName);
		}

		static inline bool IsAvailable() {
			return FModuleManager::Get().IsModuleLoaded(EditorModuleName);
		}
	};

}