/* Copyright 2021 TrackMen GmbH <mail@trackmen.de> */

#include "TrackMenCameraTrackingInterface.h"
#include "PluginLogging.h"

#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "Common/UdpSocketBuilder.h"
#include "Sockets.h"

#include <algorithm>
#include <sstream>

namespace TrackMen {

	CameraTrackingInterface::CameraTrackingInterface() {

	}

	CameraTrackingInterface::~CameraTrackingInterface() { stop_camera_tracking(); }

	TrkErrorType_t CameraTrackingInterface::check_error() {
		return m_last_error;
	}

	bool CameraTrackingInterface::got_parameters() {
		std::lock_guard<std::mutex> lock(m_params_mutex);
		return !m_params_container.empty();
	}

	bool CameraTrackingInterface::got_constants() {
		std::lock_guard<std::mutex> lock(m_constants_mutex);
		return !m_constants_container.empty();
	}

	void CameraTrackingInterface::start_camera_tracking(uint16_t port) {
		m_port = port;

		m_socket =
			FUdpSocketBuilder(FString("CameraTrackingInterface ") + FString::FromInt(m_port))
			.AsNonBlocking()
			.AsReusable()
			.BoundToPort(m_port)
			.Build();

		if (m_socket) {
			m_keep_thread_running = true;
		}

		m_receiver_worker = std::thread(std::bind(&CameraTrackingInterface::receiver_thread_func, this));
	}

	void CameraTrackingInterface::stop_camera_tracking() {
		m_keep_thread_running = false;
		while (m_is_thread_running) {
			FPlatformProcess::Sleep(0.f);
		}
		if (m_receiver_worker.joinable()) {
			m_receiver_worker.join();
		}
		close_socket();
		m_port = 0;
	}

	TrkCameraParams_t CameraTrackingInterface::get_camera_parameters() {
		std::lock_guard<std::mutex> lock(m_params_mutex);
		TrkCameraParams_t tmp;
		if (!m_params_container.empty()) {
			tmp = m_params_container.front();
			m_params_container.pop_front();
		}
		return tmp;
	}

	TrkCameraConstants_t CameraTrackingInterface::get_camera_constants() {
		std::lock_guard<std::mutex> lock(m_constants_mutex);
		TrkCameraConstants_t tmp;
		if (!m_constants_container.empty()) {
			tmp = m_constants_container.front();
			m_constants_container.pop_front();
		}
		return tmp;
	}

	void CameraTrackingInterface::close_socket() {
		if (m_socket)
		{
			m_socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_socket);
			m_socket = nullptr;
		}
	}

	void CameraTrackingInterface::receiver_thread_func() {
		m_is_thread_running = true;

		auto loopend_callback = []() {
			FPlatformProcess::Sleep(0.001f); // 1ms 
		};

		static const int MAX_DATAGRAM_SIZE = 4096;
		static const int GAME_ENGINE_MSG_BUFFERSIZE = 124;
		static const int PUBLIC_MSG_HEADERSIZE = 8;
		static const char* PUBLIC_MAGIC = "DMC01";

		uint8 buffer[MAX_DATAGRAM_SIZE];
		int32 bytes_read = 0;

		enum TrackingDataFormat {
			GameEngineOpen,
			Public,
			Unknown
		} trackingDataFormat = Unknown;

		for (; m_keep_thread_running; loopend_callback()) {

			m_socket->Recv(buffer, MAX_DATAGRAM_SIZE, bytes_read);

			while (bytes_read > 0) {

				if ((bytes_read == GAME_ENGINE_MSG_BUFFERSIZE)
					&& (*((uint32_t*)&buffer[0]) == 0x544d4531)) {
					trackingDataFormat = GameEngineOpen;
				}
				else if ((bytes_read >= PUBLIC_MSG_HEADERSIZE)
					&& (memcmp(buffer, PUBLIC_MAGIC, strlen(PUBLIC_MAGIC)) == 0)) {
					trackingDataFormat = Public;
				}
				else {
					// Not a TorqTrack packet
					trackingDataFormat = Unknown;
				}

				switch (trackingDataFormat) {
					case GameEngineOpen:      parse_game_engine_format_parameters(buffer);        break;
					case Public:              parse_public_format_parameters(buffer, bytes_read); break;
					default:                                                                      break;
				}

				m_socket->Recv(buffer, MAX_DATAGRAM_SIZE, bytes_read);
			}
		}
		m_is_thread_running = false;
	}

	void CameraTrackingInterface::parse_game_engine_format_parameters(uint8* buffer) {
		// TODO: set format

		TrkCameraParams_t tmpParams;
		TrkCameraConstants_t tmpConstants;

		int index = 8;
		tmpParams.counter = *((uint32_t*)&buffer[index]);
		index += 4;

		tmpParams.t.e.x = *((double*)&buffer[index]); // pos_x
		index += 8;
		tmpParams.t.e.y = *((double*)&buffer[index]); // pos_y
		index += 8;
		tmpParams.t.e.z = *((double*)&buffer[index]); // pos_z
		index += 8;

		tmpParams.t.e.pan = *((double*)&buffer[index]); // pan
		index += 8;
		tmpParams.t.e.tilt = *((double*)&buffer[index]); // tilt
		index += 8;
		tmpParams.t.e.roll = *((double*)&buffer[index]); // roll
		index += 8;

		tmpParams.fov = *((double*)&buffer[index]); // horiz_fov
		index += 8;

		tmpParams.centerX = *((double*)&buffer[index]); // centerX
		index += 8;
		tmpParams.centerY = *((double*)&buffer[index]); // centerY
		index += 8;

		tmpParams.k1 = *((double*)&buffer[index]); // k1
		index += 8;
		tmpParams.k2 = *((double*)&buffer[index]); // k2
		index += 8;

		tmpParams.focdist = *((double*)&buffer[index]); // focdist
		index += 8;

		tmpConstants.chipWidth = *((double*)&buffer[index]); // chipWidth
		index += 8;
		tmpConstants.chipHeight = *((double*)&buffer[index]); // chipHeight

		std::lock_guard<std::mutex> paramslock(m_params_mutex);
		std::lock_guard<std::mutex> constantslock(m_constants_mutex);
		m_params_container.push_back(tmpParams);
		m_constants_container.push_back(tmpConstants);
	}

	void CameraTrackingInterface::parse_public_format_parameters(uint8* buffer, int32 len) {
		static const int trkNetHeaderType = 6;
		static const int trkNetHeaderFormat = 7;
		static const int trkNetHeaderSize = 8;

		static const int trkMatrix = 0x0000;
		static const int trkEuler = 0x0001;

		uint8 typeChar = buffer[trkNetHeaderType];
		uint8 formatChar = buffer[trkNetHeaderFormat];

		if (typeChar == 'C') {
			// Constants
			if (formatChar == 'B') {
				// Binary format
				if (len != trkNetHeaderSize + sizeof(TrkCameraConstants_t))
					return;
				TrkCameraConstants_t tmpConstants;
				memcpy((void*)&(tmpConstants),
					(const void*)(buffer + trkNetHeaderSize),
					sizeof(TrkCameraConstants_t));
				std::lock_guard<std::mutex> lock(m_constants_mutex);
				m_constants_container.push_back(tmpConstants);
			}
			else {
				// ASCII format
				std::stringstream ss;
				ss.write((char*)(buffer + trkNetHeaderSize), len - trkNetHeaderSize);
				// Use temporary to ensure consistent data if stream fails.
				TrkCameraConstants_t tmpConst;
				ss >> tmpConst.imageWidth
					>> tmpConst.imageHeight
					>> tmpConst.blankLeft
					>> tmpConst.blankRight
					>> tmpConst.blankTop
					>> tmpConst.blankBottom
					>> tmpConst.chipWidth
					>> tmpConst.chipHeight
					>> tmpConst.fakeChipWidth
					>> tmpConst.fakeChipHeight;
				if (!ss.fail()) {
					std::lock_guard<std::mutex> lock(m_constants_mutex);
					m_constants_container.push_back(tmpConst);
				}
			}
		}
		else {
			// Parameters
			if (formatChar == 'B') {
				// Binary format
				if (len != trkNetHeaderSize + sizeof(TrkCameraParams_t))
					return;
				TrkCameraParams_t tmpParams;
				memcpy((void*)&(tmpParams),
					(const void*)(buffer + trkNetHeaderSize),
					sizeof(TrkCameraParams_t));

				std::lock_guard<std::mutex> lock(m_params_mutex);
				m_params_container.push_back(tmpParams);
			}
			else {
				// ASCII format

				// Use temporary to ensure consistent data if stream fails.
				TrkCameraParams_t tmpParams;

				std::stringstream ss;
				ss.write((char*)(buffer + trkNetHeaderSize), len - trkNetHeaderSize);

				if (buffer[trkNetHeaderSize] == 'I') {
					ss >> tmpParams.id;
				}

				ss >> std::hex >> tmpParams.format >> std::dec;
				if (tmpParams.format & trkEuler) {
					ss >> tmpParams.t.e.x
						>> tmpParams.t.e.y
						>> tmpParams.t.e.z
						>> tmpParams.t.e.pan
						>> tmpParams.t.e.tilt
						>> tmpParams.t.e.roll;
				}
				else {
					ss >> tmpParams.t.m[0][0] >> tmpParams.t.m[0][1] >> tmpParams.t.m[0][2] >> tmpParams.t.m[0][3]
						>> tmpParams.t.m[1][0] >> tmpParams.t.m[1][1] >> tmpParams.t.m[1][2] >> tmpParams.t.m[1][3]
						>> tmpParams.t.m[2][0] >> tmpParams.t.m[2][1] >> tmpParams.t.m[2][2] >> tmpParams.t.m[2][3];
					tmpParams.t.m[0][3] = tmpParams.t.m[1][3] = tmpParams.t.m[2][3] = 0.0;
					tmpParams.t.m[3][3] = 1.0;
				}
				ss >> tmpParams.fov >> tmpParams.centerX >> tmpParams.centerY
					>> tmpParams.k1 >> tmpParams.k2 >> tmpParams.focdist
					>> tmpParams.aperture >> tmpParams.counter;

				if (!ss.fail()) {
					std::lock_guard<std::mutex> lock(m_params_mutex);
					m_params_container.push_back(tmpParams);
				}
			}
		}
		
	}
}