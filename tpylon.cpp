// Copyright (c) 2016, Alexey Aristov (Suprematic UG, http://www.suprematic.net) - All rights reserved.
#include <iostream>
#include <exception>
#include <cassert>

#include <luaT.h>
#include <TH.h>

#include <sol.hpp>

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>

#include "tpylon.hpp"

namespace TP {
	Camera::Camera(sol::table params) {
		std::string type = params["type"];

		try {
			if(type == "usb") {
				Pylon::CBaslerUsbDeviceInfo info;

                std::string source = params["source"];
                if(source == "any" || source == "") {
                	_camera = new Pylon::CBaslerUsbInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice(info));
                }else{
                	info.SetSerialNumber(source.c_str());
                	_camera = new Pylon::CBaslerUsbInstantCamera(Pylon::CTlFactory::GetInstance().CreateDevice(info));
                }
			}else
			if(type == "gige") {
				Pylon::CBaslerGigEDeviceInfo info;

                std::string source = params["source"];
				if(source == "any" || source == "") {
                	_camera = new Pylon::CBaslerGigEInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice(info));
				}else{
					info.SetIpAddress(source.c_str());
                	_camera = new Pylon::CBaslerGigEInstantCamera(Pylon::CTlFactory::GetInstance().CreateDevice(info));
				}
			}else{
				throw sol::error("unsupported camera type: " + type + " (supported types are usb and gige)");
			}

			_converter.OutputPixelFormat = Pylon::PixelType_RGB8packed;
		}
		catch(Pylon::GenericException ex) {
			throw sol::error(ex.what());
		}
	}

	Camera::~Camera() {
		delete _camera;
	}

	void Camera::StartGrabbing() {
        _camera->StartGrabbing();
	}

	void Camera::StopGrabbing() {
		_camera->StopGrabbing();
	}

	bool Camera::IsGrabbing() {
		return _camera->IsGrabbing();
	}

	void Camera::CopyImage(Pylon::IImage &source, THByteTensor *destination) {
		assert(source.GetPixelType() == Pylon::PixelType_RGB8packed);

		int height = source.GetHeight();
		int width = source.GetWidth();

		THByteTensor_resize3d(destination, height, width, 3);
		memcpy(THByteTensor_data(destination), source.GetBuffer(), 3 * height * width);
	}

	void Camera::Retrieve(THByteTensor *destination) {
		Pylon::CGrabResultPtr result;

		_camera->RetrieveResult(5000, result, Pylon::TimeoutHandling_ThrowException);

		if (result->GrabSucceeded()) {
			if(!_converter.ImageHasDestinationFormat(result)) {
				Pylon::CPylonImage converted;
				_converter.Convert(converted, result);

				CopyImage(converted, destination);
			}else
				CopyImage(result, destination);				
		}
	}

	sol::object Camera::GetDeviceInfo(sol::this_state state) {
		sol::state_view lua(state);

		Pylon::CDeviceInfo info = _camera->GetDeviceInfo();

		sol::table result = lua.create_table();

		Pylon::StringList_t properties;
		info.GetPropertyNames(properties);

		for(Pylon::String_t property: properties) {
			if(info.GetPropertyAvailable(property)) {
				Pylon::String_t value;

				info.GetPropertyValue(property, value);
				result[std::string(property)] = std::string(value);
			}
		}

		return result;
	}

	sol::object Camera::GenApiGetValue(sol::this_state state, std::string name) {
		sol::state_view lua(state);
		GenApi::INode *node = _camera->GetNodeMap().GetNode(name.c_str());
		if(node != nullptr) {
			switch (node->GetPrincipalInterfaceType()) {
				case GenApi::intfIInteger:
               		return sol::object(lua, sol::in_place<int>, GenApi::CIntegerPtr(node)->GetValue());
                
                case GenApi::intfIBoolean:
               		return sol::object(lua, sol::in_place<bool>, GenApi::CBooleanPtr(node)->GetValue());

               	case GenApi::intfICommand:
               		return sol::object(lua, sol::in_place<std::string>, GenApi::CCommandPtr(node)->ToString().c_str());

               	case GenApi::intfIFloat:
               		return sol::object(lua, sol::in_place<float>, GenApi::CFloatPtr(node)->GetValue());

               	case GenApi::intfIString:
               		return sol::object(lua, sol::in_place<std::string>, GenApi::CStringPtr(node)->GetValue().c_str());

               	case GenApi::intfIEnumeration:
               	    return sol::object(lua, sol::in_place<std::string>, GenApi::CEnumerationPtr(node)->ToString().c_str());

               	default:
					throw sol::error(std::string("unsupported node type: ") + std::to_string(node->GetPrincipalInterfaceType()));
			}
		}else
			return sol::make_object(lua, sol::nil);
	}

	void Camera::GenApiSetValue(std::string name, sol::object value) {
		GenApi::INode *node = _camera->GetNodeMap().GetNode(name.c_str());

		if(node != nullptr) {
			switch (node->GetPrincipalInterfaceType()) {
				case GenApi::intfIInteger:
					GenApi::CIntegerPtr(node)->SetValue(value.as<long>());
					break;

                case GenApi::intfIBoolean:
                	GenApi::CBooleanPtr(node)->SetValue(value.as<bool>());
                	break;

               	case GenApi::intfICommand:
               		GenApi::CCommandPtr(node)->FromString(value.as<std::string>().c_str());
               		return;

               	case GenApi::intfIFloat:
               		GenApi::CFloatPtr(node)->SetValue(value.as<float>());
               		return;

             	case GenApi::intfIString:
             		GenApi::CStringPtr(node)->SetValue(value.as<std::string>().c_str());
             		return;

               	case GenApi::intfIEnumeration:
               		GenApi::CEnumerationPtr(node)->FromString(value.as<std::string>().c_str());
               		return;

               	default:
					throw sol::error(std::string("unsupported node type: ") + std::to_string(node->GetPrincipalInterfaceType()));
			}
		}
	}
}

extern "C" int luaopen_libtpylon(lua_State *state) {
	Pylon::PylonInitialize();

	sol::state_view lua(state);
	sol::table ns = lua.create_named_table("tpylon").create_named("internal");

	ns.new_usertype<TP::Camera>("camera",
		sol::constructors<sol::types<sol::table>>(),
		"StartGrabbing", &TP::Camera::StartGrabbing,
		"StopGrabbing", &TP::Camera::StopGrabbing,
		"Retrieve", &TP::Camera::Retrieve,
		"IsGrabbing", &TP::Camera::IsGrabbing,
		"GetDeviceInfo", &TP::Camera::GetDeviceInfo,
		"GenApiGetValue", &TP::Camera::GenApiGetValue,
		"GenApiSetValue", &TP::Camera::GenApiSetValue);

 	return 1;
 } 