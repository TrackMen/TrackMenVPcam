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

#include "CameraSourceFactory.h"
#include "CoreMinimal.h"
#include "LiveLinkCameraSource.h"
#include "EditorLogging.h"
#include <string>

#define LOCTEXT_NAMESPACE "TrackMenCameraSourceFactory"

FText UTrackMenCameraSourceFactory::GetSourceDisplayName() const {
	return LOCTEXT("SourceDisplayName", "TrackMen Camera");
}

FText UTrackMenCameraSourceFactory::GetSourceTooltip() const {
	return LOCTEXT("SourceTooltip", "TrackMen Camera");
}

ULiveLinkSourceFactory::EMenuType UTrackMenCameraSourceFactory::GetMenuType() const {
	return EMenuType::SubPanel;
}


TSharedPtr<ILiveLinkSource> UTrackMenCameraSourceFactory::CreateSource(const FString& ConnectionString) const {
	UE_LOG(LogTrackMenEditor, Display, TEXT("Create new live link camera source: %s"), *ConnectionString);
	TSharedPtr<TrackMen::LiveLinkCameraSource> NewSource = nullptr;
	NewSource = MakeShared<TrackMen::LiveLinkCameraSource>(
		FText::FromString("TrackMen Camera"),
		FText::FromString((std::string("UDP ") + std::to_string(FCString::Atoi(*ConnectionString))).c_str()),
		FCString::Atoi(*ConnectionString)
		);
	return NewSource;
}

TSharedPtr<SWidget> UTrackMenCameraSourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const
{
	if (!ActiveSourceEditor.IsValid()) {
		ActiveSourceEditor = SNew(STrackMenCameraSourceEditor).OnAddNewSourceClicked(STrackMenCameraSourceEditor::FOnAddNewSource::
			CreateUObject(this, &UTrackMenCameraSourceFactory::OnPanelAddNewSource, OnLiveLinkSourceCreated));
	}
	return ActiveSourceEditor;
}

void UTrackMenCameraSourceFactory::OnPanelAddNewSource(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const 
{
	//UE_LOG(LogTrackMenEditor, Display, TEXT("OnPanelAddNewSource"));
	TSharedPtr<TrackMen::LiveLinkCameraSource> NewSource = nullptr;
	if (ActiveSourceEditor.IsValid()) {
		NewSource = MakeShared<TrackMen::LiveLinkCameraSource>(
			FText::FromString("TrackMen Camera"),
			FText::FromString(("UDP " + std::to_string(ActiveSourceEditor->Port)).c_str()),
			ActiveSourceEditor->Port
			);
	}
	OnLiveLinkSourceCreated.ExecuteIfBound(NewSource, FString::FromInt(ActiveSourceEditor->Port));
}

#undef LOCTEXT_NAMESPACE
