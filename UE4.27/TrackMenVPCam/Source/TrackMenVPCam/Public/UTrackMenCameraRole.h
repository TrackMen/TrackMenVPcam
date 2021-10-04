/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "Roles/LiveLinkBasicRole.h"
#include "TrackMenCameraTrackingData.h"
#include "UTrackMenCameraRole.generated.h"

/**
* Defines a custom role for TrackMen virtual camera data.
*/
UCLASS()
class TRACKMENVPCAM_API UTrackMenCameraRole : public ULiveLinkBasicRole
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetStaticDataStruct() const override;
	virtual UScriptStruct* GetFrameDataStruct() const override;
	virtual UScriptStruct* GetBlueprintDataStruct() const override;
	virtual FText GetDisplayName() const override;

	virtual bool InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const;

};
