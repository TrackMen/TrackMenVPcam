/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceFactory.h"
#include "CameraSourceEditor.h"
#include "CameraSourceFactory.generated.h"

/**
* Creates a new UTrackMenCameraSource either from a ConnectionString
* or when STrackMenCameraSourceEditor fires the corresponding event.
*/
UCLASS()
class UTrackMenCameraSourceFactory : public ULiveLinkSourceFactory {
public:
	GENERATED_BODY()

	FText GetSourceDisplayName() const override;
	FText GetSourceTooltip() const override;
	EMenuType GetMenuType() const override;
	TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;
	TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
	void OnPanelAddNewSource(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const;

	mutable TSharedPtr<STrackMenCameraSourceEditor> ActiveSourceEditor;
};