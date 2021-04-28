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