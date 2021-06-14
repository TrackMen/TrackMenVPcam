/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

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
