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

#include <deque>
#include <mutex>
#include <stdint.h>

class FSocket;

namespace TrackMen {

	// TODO: use ring buffer for better performance
	template <typename T>
	using DequeType = std::deque<T>;

	enum TrkErrorType_t {
		TRK_ERROR_NO_ERROR,
		TRK_ERROR_CANNOT_CREATE_HANDLE_FOR_PORT,
		TRK_ERROR_NO_HANDLE_ON_THIS_PORT,
		NUM_ERRORS
	};

	union TrkTransform_t  {
		double m[4][4]; /* m [j] [i] is matrix element */
						/* in row i, column j */
		struct {
			double x;
			double y;
			double z;
			double pan;
			double tilt;
			double roll;
		} e; /* "Euler" angles */

	};

	struct TrkCameraParams_t {
		unsigned id = 0;/* == 0 if not explicitly specified */

		unsigned format = 0; /* bit mask of format options */

		TrkTransform_t t; /* coordinate transformation */
		double fov = 20.0;     /* field of view or image distance */
		double centerX = 0.0; /* center shift */
		double centerY = 0.0;

		double k1 = 0.0; /* distorsion coefficients */
		double k2 = 0.0;

		double focdist = 100000.0; /* depth of field simulation */
		double aperture = 1.0;

		unsigned long counter = 0;

	};

	struct TrkCameraConstants_t {
		unsigned id = 0; /* == 0 if not explicitly specified */

		int imageWidth = 1920;
		int imageHeight = 1080;
		int blankLeft = 0;
		int blankRight = 0;
		int blankTop = 0;
		int blankBottom = 0;

		double chipWidth = 9.6;
		double chipHeight = 5.4;
		double fakeChipWidth = 9.6;
		double fakeChipHeight = 5.4;

	};

	/**
	* Tracking interface for UDP camera data
	*/
	class CameraTrackingInterface {
	public:
		CameraTrackingInterface();
		virtual ~CameraTrackingInterface();

		TrkErrorType_t check_error();
		bool got_parameters();
		bool got_constants();

		void start_camera_tracking(uint16_t port);
		void stop_camera_tracking();
		TrkCameraParams_t get_camera_parameters();
		TrkCameraConstants_t get_camera_constants();

	private:
		void close_socket();
		void receiver_thread_func();
		void parse_game_engine_format_parameters(uint8* buffer);
		void parse_public_format_parameters(uint8* buffer, int32 len);

		uint16_t m_port = 0;

		std::thread m_receiver_worker;
		bool m_keep_thread_running = false;
		bool m_is_thread_running = false;
		FSocket* m_socket;

		DequeType<TrkCameraParams_t> m_params_container;
		DequeType<TrkCameraConstants_t> m_constants_container;
		std::mutex m_params_mutex;
		std::mutex m_constants_mutex;

		TrkErrorType_t m_last_error = TRK_ERROR_NO_ERROR;
	};
}