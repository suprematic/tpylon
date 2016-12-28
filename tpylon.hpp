// Copyright (c) 2016, Alexey Aristov (Suprematic UG, http://www.suprematic.net) - All rights reserved.
#ifndef _TPYLON_HPP
#define _TPYLON_HPP

#include <luaT.h>
#include <TH.h>

#include <sol.hpp>

#include <pylon/PylonIncludes.h>

namespace TP {
	class Camera {
	private:
		Pylon::CInstantCamera *_camera;
		Pylon::CImageFormatConverter _converter;

		void CopyImage(Pylon::IImage &source, THByteTensor *destination);

	public:
		Camera(sol::table params);
		~Camera();

		void StartGrabbing();
		void StopGrabbing();
		bool IsGrabbing();

		void Retrieve(THByteTensor *destination);

		sol::object GetDeviceInfo(sol::this_state state);
		sol::object GenApiGetValue(sol::this_state state, std::string name);
		void GenApiSetValue(std::string name, sol::object value);
	};
}
#endif