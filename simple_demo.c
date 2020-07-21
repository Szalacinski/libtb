/* This file is part of the libtb project.
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Caleb Szalacinski
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <unistd.h>
#ifdef TB_MEASURE_TIME
#include <time.h>
#endif
#include <libtb/libtb.h>
#include <libtb/protocols/serial.h>

#define SEND_TO_ALL(x) for (uint8_t __j = 1; __j <= i->num_cameras; ++__j) { uint8_t __err = x; if (__err) return __err; }
#define PT_INQ_TO_ALL() for (uint8_t __j = 1; __j <= i->num_cameras; ++__j) { uint8_t __err = tb_pt_pos_inq(i, __j, &pan_pos, &tilt_pos);\
                                                                              if (__err) return __err;\
                                                                              printf("cam: %u, pan: %u, tilt: %u\n", __j, pan_pos, tilt_pos);\
                                                                            }
#define SEND_TO_ALL_NO_ERR(x) for (uint8_t __j = 1; __j <= i->num_cameras; ++__j) { x; }
#define CAM_NUM __j

//void ir_callback(uint8_t cam_addr, uint8_t ir_id, uint8_t keycode)
//{
//	printf("IR: {CAMERA ADDRESS: %x, IR ID: %x, KEYCODE: %x}\n", cam_addr, ir_id, keycode); 
//}

//void network_change_callback(uint8_t cam_addr)
//{
//	printf("NETWORK CHANGE: {CAMERA ADDRESS: %x}\n", cam_addr);
//}

uint8_t move(struct tb_if *i)
{
	uint16_t pan_pos, tilt_pos;
	//SEND_TO_ALL_NO_ERR(tb_ir_output(i, CAM_NUM, false)); //Tandberg-specific
	//SEND_TO_ALL_NO_ERR(tb_call_led(i, CAM_NUM, true)); //Tandberg-specific
	SEND_TO_ALL(tb_pt_reset(i, CAM_NUM));
	PT_INQ_TO_ALL();
	for (uint8_t x = 0; x < 5; ++x) {
		SEND_TO_ALL(tb_pt_left(i, CAM_NUM, 0x07, 0x07));
		sleep(3);
		SEND_TO_ALL(tb_pt_stop(i, CAM_NUM));
		PT_INQ_TO_ALL();
		SEND_TO_ALL(tb_pt_right(i, CAM_NUM, 0x07, 0x07));
		sleep(3);
		SEND_TO_ALL(tb_pt_stop(i, CAM_NUM));
		PT_INQ_TO_ALL();
	}
	SEND_TO_ALL(tb_pt_reset(i, CAM_NUM));
	//SEND_TO_ALL_NO_ERR(tb_call_led(i, CAM_NUM, false)); //Tandberg-specific
	return TB_SUCCESS;
}

/* if you don't need to time packets, you can just use tb_simple_packet_wait */
uint8_t timed_packet_wait(void *interface, uint8_t cam_addr, uint8_t *read_arr)
{
	uint8_t err;
	
	#ifdef TB_MEASURE_TIME
		struct timespec ts1, ts2;
		clock_gettime(CLOCK_REALTIME, &ts1);
	#endif

	do {
		err = tb_packet_parse((struct tb_if*)interface, read_arr);
	} while (err == TB_ACK);
	
	#ifdef TB_MEASURE_TIME
		clock_gettime(CLOCK_REALTIME, &ts2);
		if (ts2.tv_nsec < ts1.tv_nsec) {
			ts2.tv_nsec += 1000000000;
			ts2.tv_sec--;
		}
		printf("%ld.%09ld\n", (long)(ts2.tv_sec - ts1.tv_sec), ts2.tv_nsec - ts1.tv_nsec);
	#endif
	
	return err;
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		fprintf(stderr, "ERROR: Needs 1 serial port argument.\n");
		return 1;
	}
	/* Set up the interface */
	/* ir_callback and network_change_callback can be set to NULL if you don't want anything to happen */
	struct tb_if interface = {tb_serial_read, tb_serial_write, timed_packet_wait,  NULL /* ir_callback */, NULL /* network_change_callback */};
	if (tb_serial_connect(&interface, argv[1])) {
		fprintf(stderr, "ERROR: Failed to open serial port.\n");
		return 1;
	}

	/* Get the number of cameras in the chain */
	if(tb_set_address(&interface)) {
		fprintf(stderr, "ERROR: Address set failed.\n");
		return 1;
	}

	uint8_t num_cameras = interface.num_cameras;

	if (num_cameras > 7 || num_cameras == 0) {
		/* Sanity check.  A malformed packet containing an incorrect address should be caught by the parser */
		fprintf(stderr, "ERROR: Number of cameras violates VISCA.\n");
		return 1;
	} else {
		printf("Address set! %u cameras in chain.\n", interface.num_cameras);
	}
	
	if (move(&interface)) {
		fprintf(stderr, "ERROR: Cmd execution failed.  Maybe an unsupported command?\n");
		return 1;
	}

	/* Cleanup */
	tb_serial_disconnect(&interface);
	return 0;
}
