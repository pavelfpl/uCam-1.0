#ifndef __UCAM_H_
#define __UCAM_H_

#include <time.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define CMD_SIZE 6

#define SNAPSHOT_TYPE_COMPRESSED	0x00 // Compressed Picture (JPEG)
#define SNAPSHOT_TYPE_UNCOMPRESSED	0x01 // Uncompressed Picture (RAW)

#define COLOR_TYPE_2B_GRAY	0x01 // 2-bit Gray Scale (RAW, 2-bit for Y only)
#define COLOR_TYPE_4B_GRAY	0x02 // 4-bit Gray Scale (RAW, 4-bit for Y only)
#define COLOR_TYPE_8B_GRAY	0x03 // 8-bit Gray Scale (RAW, 8-bit for Y only)
#define COLOR_TYPE_8B_COLOR	0x04 // 8-bit Colour (RAW, 332(RGB))
#define COLOR_TYPE_12B_COLOR	0x05 // 12-bit Colour (RAW, 444(RGB)) 
#define COLOR_TYPE_16B_COLOR	0x06 // 16-bit Colour (RAW, 565(RGB))
#define COLOR_TYPE_JPEG		0x07 // JPEG

#define RAW_RES_80x60		0x01
#define RAW_RES_160x120		0x03
#define RAW_RES_320x240		0x05
#define RAW_RES_640x480		0x07
#define RAW_RES_128x128		0x09
#define RAW_RES_128x96		0x0B

#define JPEG_RES_80x64		0x01
#define JPEG_RES_160x128	0x03
#define JPEG_RES_320x240	0x05
#define JPEG_RES_640x480	0x07

#define PIC_TYPE_SNAPSHOT	0x01
#define PIC_TYPE_RAW		0x02
#define PIC_TYPE_JPEG		0x05

#define DATA_TYPE_SNAPSHOT	0x01
#define DATA_TYPE_RAW		0x02
#define DATA_TYPE_JPEG		0x05

#define RESET_TYPE_ALL			0x00
#define RESET_TYPE_STATE_MACHINE	0x01

int wait_for_ack(int fd, unsigned char cmd_id, unsigned char *counter);
int wait_for_data(int fd, unsigned char *data, unsigned int *size);
int read_data(int fd, unsigned char *buffer);
int ucam_reset(unsigned char type, unsigned int special);
int ucam_initial(const unsigned char color,const unsigned char raw_res,const unsigned char jpeg_res);
int ucam_set_params(const unsigned char contrast,const unsigned char brightness,const unsigned char exposure);
int ucam_sleep(const unsigned char sleep_timeout);
int ucam_set_package_size();
int ucam_snapshot(unsigned char type);
int ucam_ack(unsigned int frameNumber);
int ucam_process_packet(unsigned char *buffer,struct tm *timeinfo);
int ucam_get_picture(unsigned char type);
int cmd_sync(int fd);
int cmd_set_ucam();
int cmd_take_picture();

#endif
