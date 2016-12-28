# TPylon
An experimental Lua/Torch7 wrapper around the Basler Pylon 5 library. 

While the basic code seems to work, I'd like to point out, that it still in early alpha stage. You will probably stumble over bugs.

## Current TODO list and development targets
 - [x] Test ACE, Pulse & Dart cameras
 - [x] Handle different image packing other than RGB8 - ByteTensor 3 * h * w
 - [x] Read camera info
 - [x] Read/Write GenICam properties
 - [ ] Fully expose GenICam metadata
 - [ ] Add some callbacks on events
 - [ ] Test code under Windows
 
## Simple usage example
```lua
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
```