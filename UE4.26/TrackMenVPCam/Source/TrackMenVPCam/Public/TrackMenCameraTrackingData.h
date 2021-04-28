/* Copyright 2020 TrackMen GmbH <mail@trackmen.de>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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