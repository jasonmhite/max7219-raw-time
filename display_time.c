#include "registers.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>

uint8_t buffer[8];

void set_reg(unsigned char addr, unsigned char val) {
    static unsigned char data[2];

    data[0] = addr;
    data[1] = val;
    wiringPiSPIDataRW(0, data, 2);
}

void update_display() {
    for (uint8_t i=0; i < 8; i++) {
        set_reg(8 - i, buffer[i]);
    }
}

void sync_time_2() {
    struct timeval now;
    gettimeofday(&now, NULL);

}

void sync_time() {
	struct timeval now;
    char time_buffer[20];
    struct tm* tm_info;
    char ms_buffer[20];

	gettimeofday(&now, NULL);
    
    tm_info = localtime(&now);
    strftime(time_buffer, 20, "%H%M%S", tm_info);
	sprintf(ms_buffer, "%d", now.tv_usec / 1000);

    for (uint8_t i=0; i < 6; i++) {
        buffer[i] = time_buffer[i] - '0';
        if (i == 1 || i == 3 || i == 5) { buffer[i] |= DEC; }
    }

    for (uint8_t i=0; i < 2; i++) {
        buffer[i + 6] = ms_buffer[i] - '0';	
    }
}

int main(int argc, char *argv[]) {
    wiringPiSPISetup(0, 500000); 
    
    set_reg(REG_SHUTDOWN, 0x00);
    set_reg(REG_DECODE, 0xFF);
    set_reg(REG_TEST, 0x00);
    set_reg(0x0B, 7); // Scan limit

    update_display();

    set_reg(REG_SHUTDOWN, 0x01);

    for (;;) {
         sync_time();
         update_display();
         usleep(25000);
    }
    return 1;
}
