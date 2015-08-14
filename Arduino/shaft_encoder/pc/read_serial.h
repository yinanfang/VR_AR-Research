#ifndef __READ_SERIAL_H__
#define __READ_SERIAL_H__


bool open_serial_port(const char *name, long baud);
void close_serial_port();
int get_last_serial_value();
void start_serial_read();


#endif



