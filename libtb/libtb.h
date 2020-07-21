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
#ifndef __LIBTB_H__
#define __LIBTB_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//Maximum packet return for this library.
#define TB_MAX_PACKET 16

//Return values:

#define TB_SUCCESS                    0x00

#define TB_ERROR_MESSAGE_LENGTH       0x01
#define TB_ERROR_SYNTAX               0x02
#define TB_ERROR_CMD_BUFFER_FULL      0x03
#define TB_ERROR_CMD_CANCELLED        0x04
#define TB_ERROR_NO_SOCKET            0x05
#define TB_ERROR_CMD_NOT_EXECUTABLE   0x41

#define TB_ACK                        0xD0

#define TB_ERROR_UNEXPECTED_PACKET    0xFA
#define TB_ERROR_UNKNOWN_PACKET       0xFB
#define TB_ERROR_UNDERSIZED_PACKET    0xFC
#define TB_ERROR_OVERSIZED_PACKET     0xFD
#define TB_ERROR_TIMEOUT              0xFE
#define TB_ERROR_OTHER                0xFF

struct tb_if {
	/* The protocol's read function.  Returns a positive number of read bytes, or a negative error */
	int (*read)(void* /* tb_if->connection_info */, uint8_t* /* buf */, uint8_t /* count */);
	/* The protocol's write function Returns a positive number of read bytes, or a negative error */
	int (*write)(void* /* tb_if->connection_info */, uint8_t* /* buf */, uint8_t /* count */);
	/* A function to wait for a packet before returning.  Called after every packet write. */
	uint8_t (*packet_wait)(void* /* interface */, uint8_t /* cam_addr */, uint8_t* /* read_arr */);
	/* A function to handle IR push messages.
	Keep it short, and avoid syscalls and I/O so as to not stall the parser. */
	void (*ir_callback)(uint8_t /* cam_addr */, uint8_t /* ir_id */, uint8_t /* keycode */);
	/* A function to handle Network Change push messages.
	Keep it short, and avoid syscalls and I/O so as to not stall the parser. */
	void (*network_change_callback)(uint8_t);
	/* User-defined information about the interface and connection */
	void *connection_info;
	/* User-defined information about the cameras on the interface */
	void *camera_info;
	/* The number of cameras on the interface.  Set automatically by the library's parser */
	uint8_t num_cameras;
};

/////////////
/* PARSING */
/////////////

uint8_t tb_packet_parse(struct tb_if *interface, uint8_t *read_arr); //The internal packet parser, to be called by the user.
uint8_t tb_simple_packet_wait(void *interface, uint8_t cam_addr, uint8_t *read_arr); //A simple packet_wait function.

////////////////////////
/* INTERFACE COMMANDS */
////////////////////////

uint8_t tb_set_address(struct tb_if *interface);
uint8_t tb_if_clear(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_command_cancel(struct tb_if *interface, uint8_t cam_addr, uint8_t socket);

/////////////////////
/* CAMERA COMMANDS */
/////////////////////

/* POWER */
uint8_t tb_power(struct tb_if *interface, uint8_t cam_addr, bool en);

/* MIRROR/FLIP */
uint8_t tb_mirror(struct tb_if *interface, uint8_t cam_addr, bool en);
uint8_t tb_flip(struct tb_if *interface, uint8_t cam_addr, bool en);

/* IR */
uint8_t tb_ir_output(struct tb_if *interface, uint8_t cam_addr, bool en);

/* IRIS */
uint8_t tb_iris_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_iris_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_iris_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_iris_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t iris_value);

/* AE */
uint8_t tb_ae_auto(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_ae_manual(struct tb_if *interface, uint8_t cam_addr);

/* WB */
uint8_t tb_wb_auto(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_wb_manual(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_wb_one_push(struct tb_if *interface, uint8_t cam_addr);

/* GAIN */
uint8_t tb_gain_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_gain_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_gain_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_gain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t gain_value);

/* RGAIN */
uint8_t tb_rgain_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_rgain_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_rgain_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_rgain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t rgain_value);

/* BGAIN */
uint8_t tb_bgain_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bgain_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bgain_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bgain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bgain_value);

/* BRIGHT EXP */
uint8_t tb_bright_exp(struct tb_if *interface, uint8_t cam_addr, bool en);
uint8_t tb_bright_exp_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_exp_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_exp_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_exp_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bright_exp_value);

/* BRIGHT */
uint8_t tb_bright_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_bright_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bright_value);

/* SHUTTER */
uint8_t tb_shutter_up(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_shutter_down(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_shutter_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_shutter_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t shutter_value);

/* BACKLIGHT */
uint8_t tb_backlight(struct tb_if *interface, uint8_t cam_addr, bool en);

///////////////////
/* PTZF COMMANDS */
///////////////////

/* ZOOM-FOCUS */
uint8_t tb_zoom_tele(struct tb_if *interface, uint8_t cam_addr, uint8_t zoom_speed);
uint8_t tb_zoom_tele_std(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_zoom_wide(struct tb_if *interface, uint8_t cam_addr, uint8_t zoom_speed);
uint8_t tb_zoom_wide_std(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_zoom_stop(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_zoom_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t zoom_position);
uint8_t tb_dzoom(struct tb_if *interface, uint8_t cam_addr, bool en);
uint8_t tb_zoomfocus_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t zoom_position, uint16_t focus_position);
uint8_t tb_focus_auto(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_focus_manual(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_focus_far(struct tb_if *interface, uint8_t cam_addr, uint8_t focus_speed);
uint8_t tb_focus_far_std(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_focus_near(struct tb_if *interface, uint8_t cam_addr, uint8_t focus_speed);
uint8_t tb_focus_near_std(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_focus_stop(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_focus_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t focus_position);

/* PAN-TILT */
/* pan_dir: 1 left, 2 right, 3 none */
/* tilt_dir: 1 up, 2 down, 3 none */
uint8_t tb_pt(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint8_t pan_dir, uint8_t tilt_dir);
uint8_t tb_pt_up(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_down(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_left(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_right(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_upleft(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_upright(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_downleft(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_downright(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed);
uint8_t tb_pt_stop(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_pt_absolute(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint16_t pan_position, uint16_t tilt_position);
uint8_t tb_pt_relative(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint16_t pan_position, uint16_t tilt_position);
uint8_t tb_pt_home(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_pt_reset(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_pt_limit_upright(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position);
uint8_t tb_pt_limit_downleft(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position);
uint8_t tb_pt_limit_upright_clear(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_pt_limit_downleft_clear(struct tb_if *interface, uint8_t cam_addr);

///////////////
/* INQUIRIES */
///////////////

/* Values returned in an inquiry response differ from camera to camera.  Check your command table for the values. */
uint8_t tb_cam_id_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *cam_id);
uint8_t tb_power_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *power_status);
uint8_t tb_mirror_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *mirror_status);
uint8_t tb_flip_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *flip_status);
uint8_t tb_ir_output_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *ir_output_mode);
uint8_t tb_iris_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *iris_value);
uint8_t tb_ae_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *ae_mode);
uint8_t tb_wb_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *wb_mode);
uint8_t tb_gain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *gain_value);
uint8_t tb_rgain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *rgain_value);
uint8_t tb_bgain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bgain_value);
uint8_t tb_bright_exp_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *bright_exp_mode);
uint8_t tb_bright_exp_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bright_exp_value);
uint8_t tb_bright_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bright_value);
uint8_t tb_shutter_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *shutter_value);
uint8_t tb_backlight_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *backlight_mode);
uint8_t tb_dzoom_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *dzoom_mode);
uint8_t tb_zoom_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *zoom_position);
uint8_t tb_focus_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *focus_mode);
uint8_t tb_focus_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *focus_position);
uint8_t tb_pt_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *pan_position, uint16_t *tilt_position);
uint8_t tb_video_format_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *video_format);

#ifdef __cplusplus
}
#endif
#endif /* __LIBTB_H__ */
