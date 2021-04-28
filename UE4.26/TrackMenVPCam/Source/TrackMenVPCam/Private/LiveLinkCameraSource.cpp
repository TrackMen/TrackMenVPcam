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

#include "LiveLinkCameraSource.h"
#include "PluginLogging.h"
#include "UTrackMenCameraRole.h"
#include "Misc/App.h"
#include <chrono>
#include <functional>

namespace TrackMen {

	static FTrackMenCameraFrameData GetCameraFrameFromTrkData(const TrkCameraParams_t& params,
		const TrkCameraConstants_t& constants, const FFrameRate& frameRate);

	LiveLinkCameraSource::LiveLinkCameraSource(const FText& InSourceType, const FText& InSourceMachineName, uint16_t port)
		: sourceType(InSourceType)
		, sourceMachineName(InSourceMachineName)
		, udpPort(port) {
	}

	void LiveLinkCameraSource::InitializeSettings(ULiveLinkSourceSettings* Settings) {
		// Save UDP port in connection string for recreation from presets.
		Settings->ConnectionString = FString::FromInt(udpPort);
	}

	void LiveLinkCameraSource::OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent) {
		frameRate = Settings->BufferSettings.DetectedFrameRate;
	}

	void LiveLinkCameraSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) {
		sentStaticOnce = false;
		client = InClient;
		sourceGUID = InSourceGuid;
		CreateMySubject();

		// Save time code frame rate in settings
		ULiveLinkSourceSettings* settings = Cast<ULiveLinkSourceSettings>(client->GetSourceSettings(sourceGUID));
		if ((settings != nullptr) && (settings->Mode == ELiveLinkSourceMode::Timecode)) {
			frameRate = FApp::GetTimecodeFrameRate();
			settings->BufferSettings.DetectedFrameRate = frameRate;
			// We don't transmit the source frame rate but we expect everything 
			// to run in a synchronized system, so TimecodeFrameRate should be equal 
			// to SourceTimecodeFrameRate.
			settings->BufferSettings.SourceTimecodeFrameRate = frameRate;
		}

		StartTrackingThreads();
	}

	bool LiveLinkCameraSource::IsSourceStillValid() const {
		return (client != nullptr);
	}

	bool LiveLinkCameraSource::RequestSourceShutdown() {
		// Set flag for tracking thread to stop
		keepTrackingThreadRunning = false;

		// Check if the thread has finished all live link processing
		if (!isTrackingThreadRunning) {

			// Make sure the thread is completely finished
			if (trackingThread.joinable()) {
				trackingThread.join();
			}

			// Stop the network interface.
			trackingInterface.stop_camera_tracking();
		}

		// Return false while we're still processing live link data.
		return !isTrackingThreadRunning;
	}

	inline FText LiveLinkCameraSource::GetSourceType() const {
		return sourceType;
	}

	inline FText LiveLinkCameraSource::GetSourceMachineName() const {
		return sourceMachineName;
	}

	inline FText LiveLinkCameraSource::GetSourceStatus() const {
		return sourceStatus;
	}

	void LiveLinkCameraSource::CreateMySubject() {
		// Every camera source has exactly one subject.
		// 1. Define subject data.
		// 2. Search existing subject.
		// 3. If no subject is found, create a new subject.

		// 1. Define subject data
		subjectPreset.Key = FLiveLinkSubjectKey(sourceGUID, FName(*(sourceType.ToString() + FString("-") + FString::FromInt(udpPort))));
		subjectPreset.Role = UTrackMenCameraRole::StaticClass();
		subjectPreset.bEnabled = true;

		// 2. Search existing subject.
		bool foundSubject = false;
		auto subjects = client->GetSubjects(true, true);
		for (const auto& subject : subjects) {
			if (subject == subjectPreset.Key) {
				UE_LOG(LogTrackMenPlugin, Display, TEXT("LiveLink subject already exists."));
				foundSubject = true;
				break;
			}
		}

		// 3. If no subject is found, create a new subject.
		if (!foundSubject) {
			UE_LOG(LogTrackMenPlugin, Display, TEXT("Create new LiveLink subject for source."));
			bool subjectCreated = client->CreateSubject(subjectPreset);
			if (!subjectCreated) {
				UE_LOG(LogTrackMenPlugin, Display, TEXT("Failed to create new LiveLink subject for source!"));
			}
		}
	}

	void LiveLinkCameraSource::StartTrackingThreads() {
		trackingInterface.start_camera_tracking(udpPort);
		keepTrackingThreadRunning = true;
		trackingThread = std::thread(std::bind(&LiveLinkCameraSource::TrackingThreadMain, this));
	}

	void LiveLinkCameraSource::TrackingThreadMain() {
		// This thread function polls the network interface for new tracking data.
		// Incoming data is converted and forwarded to the LiveLink client.

		// Update status flag for UI thread.
		isTrackingThreadRunning = true;

		UE_LOG(LogTrackMenPlugin, Display, TEXT("Tracking thread started"));

		// This lambda is called after every loop iteration. We sleep 1ms in order
		// in order to save some CPU load.
		auto loop_end_callback = []() {
			FPlatformProcess::Sleep(0.01f);
		};

		FTrackMenCameraFrameData frame;
		frame.chip_size = FVector2D(9.6, 5.4);

		TrkCameraConstants_t constants;
		constants.chipHeight = frame.chip_size.X;
		constants.chipWidth = frame.chip_size.Y;

		for (; keepTrackingThreadRunning; loop_end_callback()) {

			auto error = CheckTrackingInterfaceErrors();
			if (error != TrkErrorType_t::TRK_ERROR_NO_ERROR) {
				continue;
			}

			// Check data availability
			auto gotParams = trackingInterface.got_parameters();
			auto gotConstants = trackingInterface.got_constants();

			if (!gotParams) {
				continue;
			}

			// Get data
			auto params = trackingInterface.get_camera_parameters();
			if (trackingInterface.got_constants()) {
				constants = trackingInterface.get_camera_constants();
			}

			// Save previous chip size to check for changes
			FVector2D old_chip_size = frame.chip_size;

			// Convert data to LiveLink format
			frame = GetCameraFrameFromTrkData(params, constants, frameRate);

			// Push data to LiveLink client
			PushStaticToSubjectIfChipSizeChanged(old_chip_size, frame);
			PushFrameToSubject(frame);
		}

		UE_LOG(LogTrackMenPlugin, Display, TEXT("Tracking thread stopped"));

		// Update status flag for UI thread.
		isTrackingThreadRunning = false;
		return;
	}

	static FTrackMenCameraFrameData GetCameraFrameFromTrkData(const TrkCameraParams_t& params, const TrkCameraConstants_t& constants, const FFrameRate& frameRate)
	{
		// TODO: check format and convert data if necessary

		FTrackMenCameraFrameData frame;

		// Build LiveLink data format
		FVector position, tmpPosition;
		FRotator rotation, tmpRotation;

		static const unsigned trkCameraEuler = 0x0001;
		static const unsigned trkCameraY_Up = 0x0004;
		static const unsigned trkFieldOfView = 0x0010;
		static const unsigned trkVertical = 0x0020;
		static const unsigned trkDiagonal = 0x0040;

		if (!(params.format & trkCameraEuler)) {
			FMatrix m;
			m.M[0][0] = params.t.m[0][0];
			m.M[0][1] = params.t.m[0][1];
			m.M[0][2] = params.t.m[0][2];
			m.M[0][3] = params.t.m[0][3];

			m.M[1][0] = params.t.m[1][0];
			m.M[1][1] = params.t.m[1][1];
			m.M[1][2] = params.t.m[1][2];
			m.M[1][3] = params.t.m[1][3];

			m.M[2][0] = params.t.m[2][0];
			m.M[2][1] = params.t.m[2][1];
			m.M[2][2] = params.t.m[2][2];
			m.M[2][3] = params.t.m[2][3];

			m.M[3][0] = params.t.m[3][0];
			m.M[3][1] = params.t.m[3][1];
			m.M[3][2] = params.t.m[3][2];
			m.M[3][3] = params.t.m[3][3];

			tmpPosition = m.TransformPosition(FVector());
			tmpRotation = m.Rotator();
		}
		else {
			tmpPosition.X = params.t.e.x;
			tmpPosition.Y = params.t.e.y;
			tmpPosition.Z = params.t.e.z;
			tmpRotation.Yaw = params.t.e.pan;
			tmpRotation.Pitch = params.t.e.tilt;
			tmpRotation.Roll = params.t.e.roll;
		}

		// TODO: check format type
		static const bool publicFormat = true;

		if (publicFormat) {

			// Transformation with view direction X
			//if (params.format & trkCameraY_Up) {
			//	position.Z = 100.f*tmpPosition.Y;
			//	position.X = -100.f*tmpPosition.Z;
			//}
			//else {
			//	position.Z = 100.f*tmpPosition.Z;
			//	position.X = 100.f*tmpPosition.Y;
			//}
			//position.Y = 100.f*tmpPosition.X;

			// Transformation with view direction Y
			position = 100.f*tmpPosition;
			position.X = -1.f * position.X;
			rotation.Yaw = -tmpRotation.Yaw + 90.0;
			rotation.Pitch = tmpRotation.Pitch;
			rotation.Roll = tmpRotation.Roll;

		}
		else { // Unity format
			position.Y = 100.f*tmpPosition.X;
			position.Z = 100.f*tmpPosition.Y;
			position.X = 100.f*tmpPosition.Z;
			rotation.Yaw = tmpRotation.Yaw;
			rotation.Pitch = -tmpRotation.Pitch;
			rotation.Roll = -tmpRotation.Roll;
		}
		frame.Transform = FTransform(rotation, position);

		if (params.format & trkFieldOfView) {
			if (params.format & trkVertical) {
				frame.FocalLength = (float)(0.5 * constants.chipHeight / tan(PI * 0.5 * params.fov / 180.0));
			}
			else {
				frame.FocalLength = (float)(0.5 * constants.chipWidth / tan(PI * 0.5 * params.fov / 180.0));
			}
		}
		else {
			frame.FocalLength = params.fov;
		}
		frame.lens_distortion[0] = (float)params.k1;
		frame.lens_distortion[1] = (float)params.k2;

		frame.center_shift[0] = (float)params.centerX;
		frame.center_shift[1] = (float)params.centerY;

		frame.FocusDistance = (float)(params.focdist*100.0);

		frame.chip_size[0] = (float)constants.chipWidth;
		frame.chip_size[1] = (float)constants.chipHeight;

		FFrameTime time((int32)params.counter);
		frame.MetaData.SceneTime = FQualifiedFrameTime(time, frameRate);
		frame.WorldTime = FLiveLinkWorldTime();

		return frame;
	}

	TrkErrorType_t LiveLinkCameraSource::CheckTrackingInterfaceErrors()
	{
		if (!trackingInterface.got_parameters() && !trackingInterface.got_constants()) {
			auto error = trackingInterface.check_error();
			FString error_str;

			switch (error) {
			case TRK_ERROR_CANNOT_CREATE_HANDLE_FOR_PORT:
				error_str = "Cannot create handle for UDP port.";
				break;
			case TRK_ERROR_NO_HANDLE_ON_THIS_PORT:
				error_str = "No handle found for UDP port.";
				break;
			default:
				break;
			}

			if (error != TrkErrorType_t::TRK_ERROR_NO_ERROR) {
				UE_LOG(LogTrackMenPlugin, Display, TEXT("Error: %s"), *error_str);
				FPlatformProcess::Sleep(1.0f);
			}

			return error;
		}
		return TrkErrorType_t::TRK_ERROR_NO_ERROR;
	}

	void LiveLinkCameraSource::PushFrameToSubject(const FTrackMenCameraFrameData &frame)
	{
		if (client) {
			FLiveLinkFrameDataStruct frame_data_struct = FLiveLinkFrameDataStruct(FTrackMenCameraFrameData::StaticStruct());
			frame_data_struct.InitializeWith(&frame);
			client->PushSubjectFrameData_AnyThread({ sourceGUID, subjectPreset.Key.SubjectName }, MoveTemp(frame_data_struct));
		}
	}

	void LiveLinkCameraSource::PushStaticToSubjectIfChipSizeChanged(const FVector2D &old_chip_size, const FTrackMenCameraFrameData &frame)
	{
		if (!sentStaticOnce || old_chip_size.X != frame.chip_size.X || old_chip_size.Y != frame.chip_size.Y) {
			// Push the first time no matter what
			sentStaticOnce = true;
			FTrackMenCameraStaticData static_data;
			static_data.bIsFocalLengthSupported = true;
			static_data.bIsFocusDistanceSupported = true;
			static_data.FilmBackWidth = frame.chip_size.X;
			static_data.FilmBackHeight = frame.chip_size.Y;
			PushStaticToSubject(static_data);
		}
	}

	void LiveLinkCameraSource::PushStaticToSubject(const FTrackMenCameraStaticData& static_data)
	{
		if (client) {
			FLiveLinkStaticDataStruct static_data_struct = FLiveLinkStaticDataStruct(FTrackMenCameraStaticData::StaticStruct());
			static_data_struct.InitializeWith(&static_data);
			client->PushSubjectStaticData_AnyThread(subjectPreset.Key, UTrackMenCameraRole::StaticClass(), MoveTemp(static_data_struct));
		}
	}
}