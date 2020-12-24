/*
uCam III Linux user space driver
--------------------------------
Pavel Fiala - 2018 (serial.c)
--------------------------------
Inspirated by: https://github.com/yoannsculo/uCAM
*/


#include <stdio.h>
#include <unistd.h>
#include "serial.h"
#include "ucam.h"

int fd_ble;

// read_buffer function ...
// ------------------------
int read_buffer(unsigned char *buffer, unsigned char c){
	int i = 0;

	if (buffer == NULL) return -1;

	for (i=0;i<6;i++)
		buffer[i] = buffer[i+1];

	buffer[5] = c;
    
	return 0;	
}

// print_buffer function ...
// -------------------------
int print_buffer(unsigned char *buffer){
    
	int i=0;
    
	for (i=0;i<6;i++)
		printf("%.02X ", buffer[i]);
	
	printf("\n");
	return 0;
}

// send_cmd function ...
// ---------------------
int send_cmd(unsigned char *cmd){
	
    int ret=0;
    
	if (cmd == NULL)
		return -1;

	ret = write(fd, cmd, CMD_SIZE);
	
    if (ret != CMD_SIZE)
		return ret;
	else
		return 0;
}
