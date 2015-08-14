// compile with: gcc -O2 -Wall -o latency_test latency_test.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

//#define PORT "/dev/cu.usbmodem12341"
//#define PORT "/dev/cu.usbserial-A100eFbC"
//#define PORT "/dev/cu.usbserial-A9007QOH"
//#define PORT "/dev/cu.usbmodem621"
//#define PORT "/dev/ttyACM0"
//#define PORT "/dev/ttyUSB0"
#define PORT "/dev/tty.usbserial-AH01L3IU"

#define BAUD B115200

void die(const char *format, ...) __attribute__ ((format (printf, 1, 2)));


int receive_bytes(int port, int len)
{
	int r, count=0;
	int retry=0;
	char buf[512];

	if (len > sizeof(buf) || len < 1) return -1;
	// non-blocking read mode
	fcntl(port, F_SETFL, fcntl(port, F_GETFL) | O_NONBLOCK);
	while (count < len) {
		r = read(port, buf, len - count);
		//printf("read, r = %d\n", r);
		if (r < 0 && errno != EAGAIN && errno != EINTR) return -1;
		else if (r > 0) count += r;
		else {
			// no data available right now, must wait
			fd_set fds;
			struct timeval t;
			FD_ZERO(&fds);
			FD_SET(port, &fds);
			t.tv_sec = 1;
			t.tv_usec = 0;
			r = select(port+1, &fds, NULL, NULL, &t);
			//printf("select, r = %d\n", r);
			if (r < 0) return -1;
			if (r == 0) return count; // timeout
		}
		retry++;
		if (retry > 1000) return -100; // no input
	}
	fcntl(port, F_SETFL, fcntl(port, F_GETFL) & ~O_NONBLOCK);
	return count;
}

double do_test(int port, int len)
{
	char buf[8192];
	struct timeval begin, end;
	double elapsed;
	int r;

	if (len > sizeof(buf) || len < 1) return 1000000;
	memset(buf, '0', len);
	buf[len - 1] = 'x'; // end of packet marker
	// test begin
	gettimeofday(&begin, NULL);
	// send the data
	r = write(port, buf, len);
	if (r != len) die("unable to write, r = %d\n", r);
	//printf("write, r = %d\n", r);
	// receive the reply
	r = receive_bytes(port, 4);  // response is always 4 bytes
	// test end
	gettimeofday(&end, NULL);
	if (r < 1) die("error reading result, r=%d\n", r);
	elapsed = (double)(end.tv_sec - begin.tv_sec) * 1000.0;
	elapsed += (double)(end.tv_usec - begin.tv_usec) / 1000.0;
	//printf("  len=%d, elased: %.2f ms\n", len, elapsed);
	return elapsed;
}

void do_test_100_times(int port, int len)
{
	const int num = 100;
	double ms, total=0, max=0;
	int i;

	for (i=0; i<num; i++) {
		ms = do_test(port, len);
		total += ms;
		if (ms > max) max = ms;
	}
	printf("latency @ %d bytes: ", len);
	printf("%.2f ms average, ", total / num);
	printf("%.2f maximum\n", max);
}

void wait_online(int port)
{
	char x = 'x';
	int r;


	printf("waiting for board to be ready:\n");
	while (1) {
		usleep(100000);
		printf(".");
		fflush(stdout);
		r = write(port, &x, 1);
		if (r != 1) die("unable to write, r = %d\n", r);
		r = receive_bytes(port, 4);
		if (r == 4) break; // success, device online
	}
	printf("ok\n");

}

int main()
{
	printf("Program started...\n");
	int fd;
	struct termios tinfo;

	printf("Opening port...\n");
	fd = open(PORT, O_RDWR | O_NOCTTY);
	if (fd < 0) die("unable to open port %s\n", PORT);
	if (tcgetattr(fd, &tinfo) < 0) die("unable to get serial parms\n");
	if (cfsetspeed(&tinfo, B115200) < 0) die("error in cfsetspeed\n");
	if (tcsetattr(fd, TCSANOW, &tinfo) < 0) die("unable to set baud rate\n");

	printf("port %s opened\n", PORT);

	wait_online(fd);

	do_test_100_times(fd, 1);
	do_test_100_times(fd, 2);
	do_test_100_times(fd, 12);
	do_test_100_times(fd, 30);
	do_test_100_times(fd, 62);
	do_test_100_times(fd, 71);
	do_test_100_times(fd, 128);
	do_test_100_times(fd, 500);
	do_test_100_times(fd, 1000);
	do_test_100_times(fd, 2000);
	do_test_100_times(fd, 4000);
	do_test_100_times(fd, 8000);

	close(fd);
	return 0;
}

void die(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	exit(1);
}

