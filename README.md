# uCam-1.0
Fully working Linux user space driver for uCam III serial camera 

uCAM III is very nice serial JPEG Camera of 4D Systems (https://4dsystems.com.au/ucam-iii)
- Inspirated by: https://github.com/yoannsculo/uCAM
- Fully rewritten using POLLIN (better stability)
- JPEG image is periodically saved to file (format: uCAM_image_%H-%M-%S_%F.jpg)
- Tested on Raspberry Pi Zero 
- Default baud rate: 115200 (tested up 921600)
- Used as onboard camera for Strato Ballon Flights 

# Build
make 
Run: build/uCam

## Screenshots from Strato Balloon Flight
