/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CameraSourceEditor.h"
#include "CoreMinimal.h"
#include "EditorLogging.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "TrackMenCameraSourceEditor"

void STrackMenCameraSourceEditor::Construct(const FArguments& Args)  {
	onAddNewSource = Args._OnAddNewSourceClicked;
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(350)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.FillWidth(0.33f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Camera Port", "Camera Port"))
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.FillWidth(0.33f)
				[
					SNew(SNumericEntryBox<int>)
					.Value(this, &STrackMenCameraSourceEditor::GetPort)
					.OnValueChanged(this, &STrackMenCameraSourceEditor::OnPortChanged)
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.FillWidth(0.33f)
				[
					SNew(SButton)
					.Text(LOCTEXT("Add New Camera Source", "Add New Camera Source"))
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.OnClicked(this, &STrackMenCameraSourceEditor::OnAddNewSourceClicked)
				]
			]
		]
	];
}



TOptional<int> STrackMenCameraSourceEditor::GetPort() const {
	return Port;
}

void STrackMenCameraSourceEditor::OnPortChanged(const int InExampleInput) {
	Port = InExampleInput;
}

FReply STrackMenCameraSourceEditor::OnAddNewSourceClicked() const {
	onAddNewSource.ExecuteIfBound();
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE