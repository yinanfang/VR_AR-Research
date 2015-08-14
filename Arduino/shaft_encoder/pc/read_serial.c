#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <errno.h>
#include <pthread.h>

#include "read_serial.h"

bool serial_shutdown = 0;
int serial_port_fd = -1;
int serial_last_value = 0;
pthread_t serial_thread;

bool open_serial_port(const char *name, long baud) {


	struct termios tinfo;
	struct serial_struct serial_settings;
	int r;
	serial_port_fd = open(name, O_RDWR);
	if (serial_port_fd < 0) {
		printf("unable to open port %s\n", name);
		return false;
	}

	if(tcgetattr(serial_port_fd, &tinfo) < 0) {
		printf("unable to get serial parms\n");
		return false;
	}

	if(cfsetspeed(&tinfo, baud) < 0) {
		printf("error in cfsetspeed\n");
		return false;
	}

	if(tcsetattr(serial_port_fd, TCSANOW, &tinfo) < 0) {
		printf("unable to set baud rate\n");
		return false;
	}

	fcntl(serial_port_fd, F_SETFL, 0); 

	r = ioctl(serial_port_fd, TIOCGSERIAL, &serial_settings);
	if (r >= 0) {
		serial_settings.flags |= ASYNC_LOW_LATENCY;
		r = ioctl(serial_port_fd, TIOCSSERIAL, &serial_settings);
		if(r >= 0) printf("setting low latency serial mode\n");
	}

}

void close_serial_port() {
	close(serial_port_fd);
	serial_shutdown = 1;
}



int get_last_serial_value() {
	return serial_last_value;
}



void * read_serial_thread(void * args) {

	#define BUF_SIZE 16
	char buf[BUF_SIZE];
	FILE * serial_stream = fdopen(serial_port_fd, "r");
	if(serial_stream != NULL) {
		while(!serial_shutdown) {
			int i = 0;
			bool msgComplete = false;

			//keep reading 
			while(!msgComplete && i<BUF_SIZE) {
				int n = read(serial_port_fd, &buf[i], 1);
				if(n==1) {
					if(buf[i] == '\n') msgComplete = true;	
					i++;	
				} else {
					// no data available right now, wait without a busy loop with select()
					fd_set fds;
					struct timeval t;
					FD_ZERO(&fds);
					FD_SET(serial_port_fd, &fds);
					t.tv_sec = 1;
					t.tv_usec = 0;
					select(serial_port_fd+1, &fds, NULL, NULL, &t);
				}
			}
			
			if(msgComplete) {
				buf[i-2] = '\0';
				serial_last_value = atoi(buf);
				printf("read serial value %d\n", serial_last_value);
			}
		}
	} else {
		printf("could not open serial stream\n");
	}
	serial_shutdown = 0;

}

void start_serial_read() {
	pthread_create(&serial_thread, NULL, read_serial_thread, NULL);

}



#ifdef TESTING

#include <GL/glut.h>

//for latency testing, draw a white box in a single buffered window as soon as input received
void glutDisplay() {

	if(get_last_serial_value() != 0) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);

		glColor3f(1,1,1);
		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(1,0);
			glVertex2f(1,1);
			glVertex2f(0,1);
		glEnd();
	
	}
	glutSwapBuffers();
}

void glutIdle() {

	glutPostRedisplay();
}

void glInit(int argc, char *argv[]) {

	//gl init
	glutInit( &argc, argv );   
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);   
	glutInitWindowSize(640,480);    
	glutCreateWindow("Test");   
	glutDisplayFunc(glutDisplay); 
	glutIdleFunc(glutIdle);

}

int main(int argc, char *argv[]) {
	glInit(argc,argv);

	bool success = open_serial_port("/dev/ttyUSB0", 115200);
	start_serial_read();

	glClear(GL_COLOR_BUFFER_BIT);
	glutMainLoop();

	if(success) {
		pthread_join(serial_thread, NULL);
	}

}
#endif



