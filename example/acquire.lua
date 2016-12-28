-- Copyright (c) 2016, Alexey Aristov (Suprematic UG, http://www.suprematic.net) - All rights reserved.
require 'tpylon'

local cam  = tpylon.camera{type='usb'}
cam:StartGrabbing();

cam.GenApiValues.ExposureTime = 10000

local exposure = cam.GenApiValues.ExposureTime;

while cam:IsGrabbing() do
	local frame  = cam:Retrieve()

	local scaled = image.scale(frame, 1024, 768, 'simple')
	window = image.display{win=window, image=scaled}

	if exposure < 1000*1000 then
		exposure = exposure + 2000
		cam.GenApiValues.ExposureTime = exposure
	end
end		
