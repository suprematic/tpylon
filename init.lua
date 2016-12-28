-- Copyright (c) 2016, Alexey Aristov (Suprematic UG, http://www.suprematic.net) - All rights reserved.
require 'torch'
require 'image'
require 'libtpylon'

local camera = torch.class('tpylon.camera')

function camera:__init(params)
	self.driver = tpylon.internal.camera.new(params)
	self.frame  = torch.ByteTensor()
	self.GenApiValues = {}
	setmetatable(self.GenApiValues, {
		__index = 
		function(table, key) 
			return self.driver:GenApiGetValue(key)
		end,
		__newindex =
		function(table, key, value)
			self.driver:GenApiSetValue(key, value)
		end})

	self.DeviceInfo = self.driver:GetDeviceInfo()
end

function camera:StartGrabbing()
	self.driver:StartGrabbing()
end

function camera:Stop()
	self.driver:StopGrabbing()
end

function camera:IsGrabbing() 
	return self.driver:IsGrabbing()
end

function camera:Retrieve()
	local frame = self.frame
	self.driver:Retrieve(frame)
	frame = frame:permute(3,1,2)
	return frame
end

function camera:GetGenNodes()
	return self.driver:GetGenNodes()
end

function camera:GenGetNodeValue(name)
	return self.driver:GenGetNodeValue(name)
end