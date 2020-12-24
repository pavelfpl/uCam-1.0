/*
uCam III Linux user space driver
--------------------------------
Pavel Fiala - 2018 (main.c)
--------------------------------
Inspirated by: https://github.com/yoannsculo/uCAM

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "serial.h"
#include "ucam.h"

#define USB_VIRTUAL_SERIAL
#define UCAM_TTY_USB	"/dev/ttyUSB1"
#define UCAM_TTY	"/dev/ttyS0"  

#define INIT_SLEEP_TIME 2
#define SLEEP_TIME 2

int fd;
sig_atomic_t volatile g_running = 1;

// signal_handler function ...
// ---------------------------
void signal_handler(int signum){
  if (signum == SIGINT) 
      g_running = 0;
}

// Main function ...
// -----------------
int main(){
    
	struct termios tty;
	int ret = 0;
    
    signal(SIGINT, &signal_handler);

#ifdef USB_VIRTUAL_SERIAL
	if ((fd = open (UCAM_TTY_USB, O_RDWR | O_NONBLOCK)) < 0) goto err_tty;
#else
    if ((fd = open (UCAM_TTY, O_RDWR | O_NONBLOCK)) < 0) goto err_tty;
#endif
    
	tcgetattr(fd, &tty);
	tty.c_cflag = B115200 | CS8 | CREAD | CLOCAL;		// Set baud rate - defaults 115200, alternative 921600 ...
	tty.c_cflag &= (~OCRNL | ~ICRNL);		            // Very important --> 0x0D -> 0x0A conversion fix ...		
	tty.c_iflag = IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &tty);

	if ((ret = cmd_sync(fd)) < 0) {
	    printf("Couldn't sync uCam III device ...\n");
	    goto end;
	}
	
	// ucam_reset(RESET_TYPE_ALL, 0);  
	
	/*
     * The uCAM needs this time to allow its AGC and AEC circuits to stabilise
	 * otherwise the received image luminance maybe too high or too low.
     * -----------------------------------------------------------------------
	 **/
    
    sleep(INIT_SLEEP_TIME); 

	if((ret = cmd_set_ucam()) < 0) goto end;
    
    while(g_running){
        if((ret = cmd_take_picture()) < 0){
            // ----------------------------------
            // Reset and reinit camera - test ...
            // ----------------------------------
            ucam_reset(RESET_TYPE_ALL, 0);
            if ((ret = cmd_sync(fd)) < 0) break;
            sleep(INIT_SLEEP_TIME); 
            if((ret = cmd_set_ucam()) < 0) break; 
        }
        sleep(SLEEP_TIME);
    }
    
end:
	close(fd);
	return 0;
    
err_tty:
#ifdef USB_VIRTUAL_SERIAL
    printf("Couldn't open serial device: %s\n", UCAM_TTY_USB);
#else
    printf("Couldn't open serial device: %s\n", UCAM_TTY);
#endif
	return 1;
}
