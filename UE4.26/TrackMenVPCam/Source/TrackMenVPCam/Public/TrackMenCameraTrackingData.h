/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "Roles/LiveLinkTransformTypes.h"
#include "TrackMenCameraTrackingData.generated.h"

/**
* Dynamic camera tracking data
*/
USTRUCT(BlueprintType)
struct TRACKMENVPCAM_API FTrackMenCameraFrameData : public FLiveLinkCameraFrameData
{
	// Unreal Header Tool does not work with namespaces -> FTrackMenClassName
	GENERATED_BODY()

	/**
	* Centered lens distortion parameters (inverse transform, r in mm)
	* r' = 1 + r * k1 + r²*k2
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TrackMen)
		FVector2D lens_distortion;

	/**
	* X/Y center shift parameters in mm (in chip space)
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TrackMen)
		FVector2D center_shift;

	/**
	* Chip size in mm
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TrackMen)
		FVector2D chip_size;
};


/**
* Static camera data
*/
USTRUCT(BlueprintType)
struct TRACKMENVPCAM_API FTrackMenCameraStaticData : public FLiveLinkCameraStaticData
{
	// Unreal Header Tool does not work with namespaces -> FTrackMenClassName
	GENERATED_BODY()
};

/**
* FTrackMenCameraBlueprintData makes frame and static camera tracking data
* available in blueprints.
*/
USTRUCT(BlueprintType)
struct TRACKMENVPCAM_API FTrackMenCameraBlueprintData : public FLiveLinkBaseBlueprintData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackMen")
		FTrackMenCameraStaticData StaticData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackMen")
		FTrackMenCameraFrameData FrameData;
};