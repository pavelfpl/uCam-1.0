#ifndef __SERIAL_H_
#define __SERIAL_H_

extern int fd;

int read_buffer(unsigned char *buffer, unsigned char c);
int print_buffer(unsigned char *buffer);
int send_cmd(unsigned char *cmd);

#endif
