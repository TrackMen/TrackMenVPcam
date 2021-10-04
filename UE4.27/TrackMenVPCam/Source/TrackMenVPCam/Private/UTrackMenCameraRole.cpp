/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#include "UTrackMenCameraRole.h"

#define LOCTEXT_NAMESPACE "TrackMenCameraRole"

UScriptStruct* UTrackMenCameraRole::GetStaticDataStruct() const 
{ 
	return FTrackMenCameraStaticData::StaticStruct(); 
}

UScriptStruct* UTrackMenCameraRole::GetFrameDataStruct() const 
{ 
	return FTrackMenCameraFrameData::StaticStruct(); 
}

UScriptStruct* UTrackMenCameraRole::GetBlueprintDataStruct() const
{
	return FTrackMenCameraBlueprintData::StaticStruct();
}

FText UTrackMenCameraRole::GetDisplayName() const 
{ 
	return LOCTEXT("TrackMenCameraRole", "TrackMen"); 
}

bool UTrackMenCameraRole::InitializeBlueprintData(const FLiveLinkSubjectFrameData& InSourceData, FLiveLinkBlueprintDataStruct& OutBlueprintData) const
{
	bool bSuccess = false;

	FTrackMenCameraBlueprintData* BlueprintData = OutBlueprintData.Cast<FTrackMenCameraBlueprintData>();
	const FTrackMenCameraStaticData* StaticData = InSourceData.StaticData.Cast<FTrackMenCameraStaticData>();
	const FTrackMenCameraFrameData* FrameData = InSourceData.FrameData.Cast<FTrackMenCameraFrameData>();
	if (BlueprintData && StaticData && FrameData)
	{
		GetStaticDataStruct()->CopyScriptStruct(&BlueprintData->StaticData, StaticData);
		GetFrameDataStruct()->CopyScriptStruct(&BlueprintData->FrameData, FrameData);
		bSuccess = true;
	}

	return bSuccess;
}

#undef LOCTEXT_NAMESPACE