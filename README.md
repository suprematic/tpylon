# TPylon
An experimental [Torch7](http://torch.ch/) (Lua) client for [Basler](http://www.baslerweb.com) machine vision cameras. 

While the basic code seems to work, I'd like to point out, that it still in early alpha stage. You will probably stumble over bugs.

## Current TODO list and development targets
 - [x] Tested with ACE, Pulse & Dart cameras
 - [ ] Handle pixel formats other than RGB8 (ByteTensor 3/h/w)
 - [x] Read camera info
 - [x] Read/Write GenICam properties
 - [ ] Fully expose GenICam metadata
 - [ ] Add some callbacks on events
 - [ ] Test code under Windows
 
## Simple usage example
```lua
require 'tpylon'

-- connect to first USB camera and start grabbing
local cam  = tpylon.camera{type='usb'}
cam:StartGrabbing();

cam.GenApiValues.ExposureTime = 10000

-- read current ExposureTime settings
local exposure = cam.GenApiValues.ExposureTime;

while cam:IsGrabbing() do
        -- retrieve next frame from the camera
	local frame  = cam:Retrieve()

        -- rescale and display image
	local scaled = image.scale(frame, 1024, 768, 'simple')
	window = image.display{win=window, image=scaled}

        -- increase exposure time by 2ms
	if exposure < 1000*1000 then
		exposure = exposure + 2000
		cam.GenApiValues.ExposureTime = exposure
	end
end		
```
