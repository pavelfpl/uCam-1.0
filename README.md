
# uCam-1.0
Fully working Linux **user space** driver for **uCam III serial camera** 

**uCAM III** is very nice  JPEG serial Camera of 4D Systems (https://4dsystems.com.au/ucam-iii)
- Inspirated by: https://github.com/yoannsculo/uCAM
- Fully rewritten using `POLLIN` (better stability)
- JPEG image is periodically saved to file (format: `uCAM_image_%H-%M-%S_%F.jpg`)
- Tested on **Raspberry Pi Zero** and **FTDI USB** to serial converter 
- Default baud rate: 115200Bd (tested up to 921600Bd)
- Used as onboard camera for **Strato Ballon Flights**
- Max resolution: **640x480**
- Framerate is about 1 picture per second (115200Bd)

## Build
Change serial `dev`  define in  `main.c ` (e.g. `#define UCAM_TTY "/dev/ttyS0"` or `#define UCAM_TTY_USB "/dev/ttyUSB1"`)

`make` 

Run:`build/uCam`

## Screenshots from Strato Balloon Flight
![Strato flight 1](https://github.com/pavelfpl/uCam-1.0/blob/master/uCAM_image_21-41-13_2018-11-30.jpg)
![Strato flight 2](https://github.com/pavelfpl/uCam-1.0/blob/master/uCAM_image_22-24-29_2018-09-21.jpg)
![Strato flight 3](https://github.com/pavelfpl/uCam-1.0/blob/master/uCAM_image_23-32-35_2018-11-30.jpg)

