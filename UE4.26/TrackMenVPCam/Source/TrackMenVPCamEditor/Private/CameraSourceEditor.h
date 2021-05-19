/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "Widgets/Text/STextBlock.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

static constexpr int TRACKMEN_CAMERA_DEFAULT_PORT = 60005;

/**
* This class defines the UI widget that is shown in the LiveLink
* screen to create a new camera source.
*/
class STrackMenCameraSourceEditor : public SCompoundWidget{
public:
	DECLARE_DELEGATE(FOnAddNewSource);

	SLATE_BEGIN_ARGS(STrackMenCameraSourceEditor){}
		SLATE_EVENT(FOnAddNewSource, OnAddNewSourceClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	FReply OnAddNewSourceClicked() const;

	int Port = TRACKMEN_CAMERA_DEFAULT_PORT;

private:
	TOptional<int> GetPort() const;

	void OnPortChanged(const int InExampleInput);

	FOnAddNewSource onAddNewSource;
};