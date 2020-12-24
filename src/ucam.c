/*
uCam III Linux user space driver
--------------------------------
Pavel Fiala - 2018 (ucam.c)
--------------------------------
Inspirated by: https://github.com/yoannsculo/uCAM
*/

#include "ucam.h"
#include "serial.h"

#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

#define PACKET_SIZE 512
#define REPEAT_TIMEOUT 1000
#define POLL_TIMEOUT 500

// ----------------------------------------------------------------------------
// Following commands are only structure basis, fields need to be filled up ...
// ----------------------------------------------------------------------------
const unsigned char CMD_SYNC[] = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
const unsigned char ACK[] = {0xAA, 0x0E, 0x00, 0x00, 0x00, 0x00};
const unsigned char INITIAL[] = {0xAA, 0x01, 0x00, 0x07, 0x07, 0x07};
const unsigned char SET_PACKAGE_SIZE[]	= {0xAA, 0x06, 0x08, 0x00, 0x00, 0x00};
const unsigned char SNAPSHOT[] = {0xAA, 0x05, 0x00, 0x00, 0x00, 0x00};
const unsigned char GET_PICTURE[] = {0xAA, 0x04, 0x01, 0x00, 0x00, 0x00};
const unsigned char DATA[] = {0xAA, 0x0A, 0x01, 0x00, 0x00, 0x00};
const unsigned char RESET[] = {0xAA, 0x08, 0x00, 0x00, 0x00, 0x00};
const unsigned char CONTRAST_ETC[] = {0xAA, 0x14, 0x00, 0x00, 0x00, 0x00};
const unsigned char SLEEP[] = {0xAA, 0x15, 0x00, 0x00, 0x00, 0x00};

int fd;

// wait_for_ack function ...
// -------------------------
int wait_for_ack(int fd, unsigned char cmd_id, unsigned char *counter){
  
    int i = 0;
    int res = 0;
    
	unsigned char c;
	unsigned char buffer[6];
	unsigned char ack[CMD_SIZE];

	memcpy(ack, ACK, CMD_SIZE);
	ack[2] = cmd_id;
	
	struct pollfd ufds;   // Set up poll file descriptors ...
	ufds.fd = fd;         // Attach socket to watch ...
	ufds.events = POLLIN; // Set events to notify on ...
	
	for (i=0;i<REPEAT_TIMEOUT;i++) { 
	     res = poll(&ufds, 1, POLL_TIMEOUT);
	    
	     if(res > 0){
            read(fd, &c, 1);
            read_buffer(buffer, c);
            // print_buffer(buffer);
            
           if (buffer[0] == ack[0] && buffer[1] == ack[1] && buffer[2] == ack[2] && ack[4] == 0x00 && ack[5] == 0x00){
               *counter = buffer[3];
               return 0;
           }
        }
    }

    return -1;
}

// wait_for_data function ...
// --------------------------
int wait_for_data(int fd, unsigned char *data, unsigned int *size){
	
    int i = 0;
    int res = 0; 
    
	unsigned char c;
	unsigned char buffer[6];
	struct pollfd ufds;    // Set up poll file descriptors ...
	ufds.fd = fd;          // Attach socket to watch ...
	ufds.events = POLLIN;  // Set events to notify on ...

	for (i=0;i<REPEAT_TIMEOUT;i++){
	    res = poll(&ufds, 1, POLL_TIMEOUT);
	    
	    if(res > 0){
           read(fd, &c, 1);
           read_buffer(buffer, c);
		   // print_buffer(buffer);
		
           if (buffer[0] == data[0] && buffer[1] == data[1] && buffer[2] == data[2]){ // *counter = buffer[3];
			   // printf("size : %.02X %.02X %.02X\n", buffer[3], buffer[4], buffer[5]);
			   *size = (buffer[5] << 16 | buffer[4]<< 8 | buffer[3]);
               printf("uCAM III ACK received ... \n");
			   return 0;
           }
        }
	}
	return -1;
}

// read_data function ...
// ----------------------
int read_data(int fd, unsigned char *buffer){
    
    int i = 0;
    int res = 0; 
    
    struct pollfd ufds;   // Set up poll file descriptors ...
    ufds.fd = fd;         // Attach socket to watch ...
    ufds.events = POLLIN; // Set events to notify on ...
  
    for(i = 0;i < PACKET_SIZE;i++){
        res = poll(&ufds, 1, POLL_TIMEOUT);
    
        if(res > 0){
           read(fd, buffer+i, 1);
        }else{
           printf("uCam III end of data ... %d\n",i);
           return 1; // 1 - means here no error ...  
        }
    }
    
    printf("uCam III data packet ready ... \n");
    return 0;
}

// ucam_reset function ...
// -----------------------
int ucam_reset(unsigned char type, unsigned int special){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter = 0;
	
	printf("Sending RESET uCAM III cmd ......\n");

	memcpy(cmd, RESET, CMD_SIZE);
	cmd[3] = type;
    
	if(special) cmd[5] = 0xFF;
	write(fd,cmd,CMD_SIZE);

	if((ret = wait_for_ack(fd, RESET[1], &ack_counter)) < 0) goto err;

	printf("uCAM III received %.02X\n", ack_counter);
	return 0;
err:
	printf("Error - receiving RESET uCAM III cmd ...\n");
	return ret;
}

// ucam_initial function ...
// -------------------------
int ucam_initial(const unsigned char color,const unsigned char raw_res,const unsigned char jpeg_res){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter = 0;
	
	printf("Sending INITIAL uCAM III cmd ...\n");

	memcpy(cmd, INITIAL, CMD_SIZE);
	cmd[3] = color;    // Color ...
	cmd[4] = raw_res;  // RAW resollution ... 
	cmd[5] = jpeg_res; // JPEG resolution ...

	if((ret = send_cmd(cmd)) < 0) goto err;
	if((ret = wait_for_ack(fd,INITIAL[1],&ack_counter) < 0)) goto err;

	printf("uCAM III received %.02X\n",ack_counter);
	return 0;
err:
	printf("Error - receiving INITIAL uCAM III cmd ...\n");
	return ret;
}

// ucam_set_params function ...
// ----------------------------
int ucam_set_params(const unsigned char contrast,const unsigned char brightness,const unsigned char exposure){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter = 0;
	
	printf("Sending CONTRAST/BRIGHTNESS/EXPOSURE uCAM III cmd ...\n");

	memcpy(cmd, CONTRAST_ETC, CMD_SIZE);
	cmd[2] = contrast;    // Contrast ...
	cmd[3] = brightness;  // Brightness ... 
	cmd[4] = exposure;    // Exposure ...

	if((ret = send_cmd(cmd)) < 0) goto err;
	if((ret = wait_for_ack(fd,CONTRAST_ETC[1],&ack_counter) < 0)) goto err;

	printf("uCAM III received %.02X\n",ack_counter);
	return 0;
err:
	printf("Error - receiving CONTRAST/BRIGHTNESS/EXPOSURE uCAM III cmd ...\n");
	return ret;
}

// ucam_sleep function ...
// -----------------------
int ucam_sleep(const unsigned char sleep_timeout){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter = 0;
	
	printf("Sending SLEEP uCAM III cmd ...\n");

	memcpy(cmd, SLEEP, CMD_SIZE);
	cmd[2] = sleep_timeout;    // sleep timout ...

	if((ret = send_cmd(cmd)) < 0) goto err;
	if((ret = wait_for_ack(fd,SLEEP[1],&ack_counter) < 0)) goto err;

	printf("uCAM III received %.02X\n",ack_counter);
	return 0;
err:
	printf("Error - receiving SLEEP uCAM III cmd ...\n");
	return ret;
}

// ucam_set_package_size function ...
// ----------------------------------
int ucam_set_package_size(){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter;
	
	printf("Sending SET_PACKAGE_SIZE uCAM III cmd...\n");

	memcpy(cmd, SET_PACKAGE_SIZE, CMD_SIZE);
	cmd[3] = 0x00;          // 512 bytes == 0x002 ...
	cmd[4] = 0x02;

	if((ret = send_cmd(cmd)) < 0) goto err;
	if((ret = wait_for_ack(fd, SET_PACKAGE_SIZE[1], &ack_counter)) < 0) goto err;

	printf("uCAM III received %.02X\n", ack_counter);
	return 0;
err:
	printf("Error - receiving SET_PACKAGE_SIZE uCAM III cmd ...\n");
	return ret;
}

// ucam_snapshot function ...
// --------------------------
int ucam_snapshot(unsigned char type){
  
    int ret = 0;
    
	unsigned char cmd[CMD_SIZE];
	unsigned char ack_counter = 0;
	
	printf("Sending SNAPSHOT uCAM III cmd ...\n");

	memcpy(cmd, SNAPSHOT, CMD_SIZE);
	cmd[2] = type;
	cmd[3] = 0x00;
	cmd[4] = 0x00;

	if((ret = send_cmd(cmd)) < 0) goto err;
	if((ret = wait_for_ack(fd, SNAPSHOT[1], &ack_counter)) < 0) goto err;

	printf("received %.02X\n", ack_counter);
	return 0;
err:
	printf("Error - receiving SNAPSHOT uCAM III cmd ...\n");
	return ret;
}

// ucam_ack function ...
// ---------------------
int ucam_ack(unsigned int frameNumber){
  
	unsigned char cmd[CMD_SIZE];
	int ret = 0;

	printf("Sending ACK uCAM III cmd ...\n");

	memcpy(cmd, ACK, CMD_SIZE);
	cmd[4] = frameNumber & 0xFF;
	cmd[5] = ((frameNumber >> 8) & 0xFF);

	if ((ret = send_cmd(cmd)) < 0) goto err;
	
	return 0;
err:
	printf("Error - receiving ACK uCAM III cmd ...\n");
	return ret;
}

// ucam_process_packet function ...
// --------------------------------
int ucam_process_packet(unsigned char *buffer,struct tm *timeinfo){
    
    FILE *fptr;
    char image[80];
    
    // unsigned int test_number = (buffer[1]<<8 | buffer[0]);   
    unsigned int size = (buffer[3]<<8 | buffer[2]);  
    
    // First (initial) image format ...
    // --------------------------------
    // sprintf(image, "uCAM_image_%d.jpg", imageNumber);
    
    // Second (with time) image format ...
    // -----------------------------------
    strftime(image,80,"uCAM_image_%H-%M-%S_%F.jpg",timeinfo);
    
    if ((fptr = fopen(image,"ab")) == NULL){
        printf("Error opening file %s ...",image);
        return -1;
    }
    
    fwrite(buffer+4,sizeof(char),size,fptr);  
    fclose(fptr);
    
    return 0;
}

// ucam_get_picture function ...
// -----------------------------
int ucam_get_picture(unsigned char type){
  
    int i = 0;
    int ret = 0;
	int numPackets = 0;
    unsigned int size = 0;
    
    unsigned char ack_counter;
	unsigned char cmd[CMD_SIZE];
	unsigned char buffer[512];
    
    time_t rawtime;
    struct tm *timeinfo = 0;

	printf("Sending uCAM III GET_PICTURE cmd ...\n");

	memcpy(cmd, GET_PICTURE, CMD_SIZE);
    cmd[2] = type;

	if ((ret = send_cmd(cmd)) < 0) goto err;
	if ((ret = wait_for_ack(fd, GET_PICTURE[1], &ack_counter)) < 0) goto err;

	printf("uCAM III received %.02X\n", ack_counter);

	memcpy(cmd, DATA, CMD_SIZE);
	cmd[2] = type;

	if ((ret = wait_for_data(fd,cmd,&size)) < 0) goto err;
	
    // Calculate numPackets ...
    // ------------------------
	numPackets = size/(PACKET_SIZE-6);
	if((size % (512-6))!=0) numPackets+=1;
	
    // Get current time ...
    // --------------------
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Set ucam_ack ...
    // ----------------
	ucam_ack(0);
	
	for(i=1;i<=numPackets;i++){
	    printf("Processing uCAM III packet %d ...\n",i);
	    read_data(fd,buffer);
	    ucam_process_packet(buffer,timeinfo);
	    ucam_ack(i);
	}
	
	return 0; 
err:
	printf("Error - receiving GET PICTURE uCAM III cmd ...\n");
	return ret;
}

// cmd_sync function ...
// ---------------------
int cmd_sync(int fd){
 
	unsigned char buffer[6];       // unsigned char c;
	unsigned char c;
	int attempt = 0;
	int res = 0; 
	int attemp_inc = 5000;

	int ack_received = 0;
	unsigned char ack_counter = 0;

	struct pollfd ufds;    // Set up poll file descriptors ...
	ufds.fd = fd;          // Attach socket to watch ...
	ufds.events = POLLIN;  // Set events to notify on ...
		
	while (attempt < 60) {
		printf("uCam III synchronization attempt: %d\n", attempt);
		write(fd, CMD_SYNC, ARRAY_SIZE(CMD_SYNC));

		res = poll(&ufds, 1, POLL_TIMEOUT);
		if (res > 0) {        // If data was received ...
		    read(fd, &c, 1);
            
		    // Only for debug - disable in production ...
		    // ------------------------------------------
		    // printf("%.02X ", c);
		    read_buffer(buffer, c);
		    print_buffer(buffer);
			
		    if (buffer[0] == 0xAA && buffer[1] == 0x0E &&
			    buffer[2] == 0x0D && buffer[4] == 0x00 &&
                buffer[5] == 0x00){
                ack_counter = buffer[3];
                read(fd, buffer, 6);
            
                if (buffer[0] == 0xAA && buffer[1] == 0x0D &&
                    buffer[2] == 0x00 && buffer[3] == 0x00 &&
                    buffer[4] == 0x00 && buffer[5] == 0x00) {
                    ack_received = 1;
                    print_buffer(buffer);
                    break;
                }
		    }
		    
		    // printf("\n");
		}
		
		if(ack_received == 1) break;
		attempt++;
		usleep(attemp_inc); attemp_inc+=1000;
	}
	
	if(!ack_received) return -1;
	printf("uCam III ACK received ... \n");

	unsigned char cmd[6];
	memcpy(cmd, ACK, 6);
	cmd[2] = 0x0D;
	cmd[3] = ack_counter;

	write(fd, cmd, 6);
	return 0;
}

// cmd_set_ucam function ...
// -------------------------
int cmd_set_ucam(){
    
	int ret;

	if ((ret = ucam_initial(0x07,0x07,0x07)) < 0) goto err;
    if ((ret = ucam_set_params(0x02,0x02,0x02)) < 0) goto err;
    if ((ret = ucam_sleep(0x00)) < 0) goto err;
	if ((ret = ucam_set_package_size()) < 0) goto err;

	return 0;
err:
	printf("ERROR - setting uCAM III camera !\n");
	return ret;
}

// cmd_take_picture function ...
// -----------------------------
int cmd_take_picture(){
	
	int ret;
    
    if((ret = ucam_snapshot(0x00)) < 0) goto err;
    
    usleep(500000); // sleep 500 ms ...
    
    if ((ret = ucam_get_picture(0x01)) < 0) goto err;

	return 0;
err:
	printf("ERROR - getting uCAM III image !\n");
	return ret;
}

