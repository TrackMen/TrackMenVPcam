/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

namespace TrackMen {

	/**
	* Defines the Editor module.
	*/
	class EditorModule : public IModuleInterface {
	public:
		// WARNING: if you change MyModuleName here, also update it in the .cpp file.
        //          Necessary because of Unreal macro idiosyncrasy.
		static constexpr char* MyModuleName = "TrackMenVPCamEditor";

		static inline EditorModule& Get() {
			return FModuleManager::LoadModuleChecked< EditorModule >(MyModuleName);
		}

		static inline bool IsAvailable() {
			return FModuleManager::Get().IsModuleLoaded(MyModuleName);
		}
	};

}