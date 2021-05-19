/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkComponentController.h"

#include "UTrackMenLiveLinkCameraControllerComponent.generated.h"

/**
* Defines the actual component that can be attached to a camera actor.
* Currently this does not provide any additional functionality than the
* standard ULiveLinkComponentController. It's just a placeholder for
* future changes.
*/
UCLASS(ClassGroup = (Custom), meta = (DisplayName = "TrackMen Live Link Camera Controller Component", BlueprintSpawnableComponent))
class TRACKMENVPCAM_API UTrackMenLiveLinkCameraControllerComponent : public ULiveLinkComponentController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, DisplayName = "Enable transform", Category = "TrackMen")
		bool EnableTransform = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable chip size", Category = "TrackMen")
		bool EnableChipSize = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable center shift", Category = "TrackMen")
		bool EnableCenterShift = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable lens distortion", Category = "TrackMen")
		bool EnableLensDistortion = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable focal length", Category = "TrackMen")
		bool EnableFocalLength = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable aperture", Category = "TrackMen")
		bool EnableAperture = true;
	UPROPERTY(EditAnywhere, DisplayName = "Enable focus distance", Category = "TrackMen")
		bool EnableFocusDistance = true;

	UTrackMenLiveLinkCameraControllerComponent();
};