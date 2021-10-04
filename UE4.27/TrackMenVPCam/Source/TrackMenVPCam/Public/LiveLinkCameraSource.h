/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "LiveLinkClient.h"
#include "TrackMenCameraTrackingInterface.h"
#include <thread>
#include <mutex>

struct FLiveLinkSubjectPreset;
struct FTrackMenCameraFrameData;
struct FTrackMenCameraStaticData;

namespace TrackMen {

	/**
	* LiveLinkCameraSource feeds tracking data of one virtual camera
	* into the LiveLink system.
	*/
	class TRACKMENVPCAM_API LiveLinkCameraSource : public ILiveLinkSource {
	public:
		LiveLinkCameraSource(const FText& InSourceType, const FText& InSourceMachineName, uint16_t port);
		virtual ~LiveLinkCameraSource() {}

		// ILiveLinkSource Interface
		void InitializeSettings(ULiveLinkSourceSettings* Settings) override;
		void OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent) override;
		void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
		bool IsSourceStillValid() const override;
		bool RequestSourceShutdown() override;
		FText GetSourceType() const override;
		FText GetSourceMachineName() const override;
		FText GetSourceStatus() const override;
		TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override { return ULiveLinkSourceSettings::StaticClass(); }

	private:
		void CreateMySubject();

		FText sourceType;
		FText sourceMachineName;
		FText sourceStatus;
		FName subjectName;
		ILiveLinkClient* client;
		FGuid sourceGUID;

		// Tracking infrastructure methods
		void StartTrackingThreads();
		void TrackingThreadMain();
		TrkErrorType_t CheckTrackingInterfaceErrors();
		void PushFrameToSubject(const FTrackMenCameraFrameData &frame);
		void PushStaticToSubjectIfChipSizeChanged(const FVector2D &old_chip_size, const FTrackMenCameraFrameData &frame);
		void PushStaticToSubject(const FTrackMenCameraStaticData& static_data);

		// Tracking infrastructure members
		bool keepTrackingThreadRunning = false;
		bool isTrackingThreadRunning = false;
		std::thread trackingThread;
		uint16_t udpPort = 0;
		CameraTrackingInterface trackingInterface;
		FLiveLinkSubjectPreset subjectPreset;
		FFrameRate frameRate;
		bool sentStaticOnce = false;
	};

}