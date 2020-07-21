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
#include <libtb/libtb.h>
#include <libtb/internal.h>

/////////////
/* PARSING */
/////////////

uint8_t tb_packet_parse(struct tb_if *interface, uint8_t *read_arr)
{
	while (1) {
		uint8_t packet_len = 0;
		while (1) {
			int err = interface->read(interface->connection_info, &read_arr[packet_len], 1);

			if (err == 0) {
				return TB_ERROR_TIMEOUT;
			} else if (err < 0) {
				return TB_ERROR_OTHER;
			}

			++packet_len;

			if (read_arr[packet_len - 1] == 0xFF) {
				break;
			} else if (packet_len >= TB_MAX_PACKET) {
				return TB_ERROR_OVERSIZED_PACKET;
			}
		}

		if ((packet_len >= 3) && ((read_arr[1] & 0xF0) == 0x50)) { //complete or inquiry return

			return TB_SUCCESS;

		} else if ((packet_len == 3 && ((read_arr[1] & 0xF0) == 0x40))){ //ACK.  Many Tandberg cameras do not use this.

			return TB_ACK;

		} else if ((packet_len == 7) && (read_arr[1] == 0x07) && (read_arr[2] == 0x7D) && (read_arr[3] == 0x02)){ //IR push message
			if (interface->ir_callback) {
				interface->ir_callback((read_arr[0] >> 4) - 0x08, read_arr[4], read_arr[5]);
			}
			continue;

		} else if ((packet_len == 3) && (read_arr[1] == 0x38)){ //camera added or removed from chain
			if (interface->network_change_callback) {
				interface->network_change_callback((read_arr[0] >> 4) - 0x08);
			}
			continue;

		} else if (packet_len < 3) { //undersized packet

			return TB_ERROR_UNDERSIZED_PACKET;

		} else if ((packet_len == 4) && ((read_arr[1] & 0xF0) == 0x60)){ //error

			return read_arr[2];

		} else if ((packet_len == 4) && (read_arr[0] == 0x88) && (read_arr[1] == 0x30)) {

			uint8_t num_cameras = read_arr[2] - 1;
			if ((num_cameras > 7) || (num_cameras == 0)) {
				return TB_ERROR_UNKNOWN_PACKET;
			} else {
				interface->num_cameras = num_cameras;
				return TB_SUCCESS;
			}

		} else if (read_arr[0] == 0x88) { //broadcast

			return TB_SUCCESS;

		} else {

			return TB_ERROR_UNKNOWN_PACKET;

		}
	}
}

uint8_t tb_simple_packet_wait(void *interface, uint8_t cam_addr, uint8_t *read_arr)
{
	uint8_t err;

	do {
		err = tb_packet_parse((struct tb_if*)interface, read_arr);
	} while (err == TB_ACK);

	return err;
}

////////////////////////
/* INTERFACE COMMANDS */
////////////////////////

uint8_t tb_set_address(struct tb_if *interface)
{
	INIT_PACKET(0x30, 0x01);
	return SEND_BROADCAST();
}

uint8_t tb_if_clear(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x00, 0x01);
	return SEND_BROADCAST();
}

uint8_t tb_command_cancel(struct tb_if *interface, uint8_t cam_addr, uint8_t socket)
{
	INIT_PACKET(0x20 | (0x01 & socket));
	return SEND_COMMAND();
}


/////////////////////
/* CAMERA COMMANDS */
/////////////////////

/* POWER */
uint8_t tb_power(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x00, en);
}


/* MIRROR/FLIP */
uint8_t tb_mirror(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x61, en);
}

uint8_t tb_flip(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x66, en);
}


/* IR */
uint8_t tb_ir_output(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_cmd(interface, cam_addr, 0x06, 0x08, ENABLE(en));
}

/* IRIS */
uint8_t tb_iris_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0b, 0x02);
}

uint8_t tb_iris_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0b, 0x03);
}

uint8_t tb_iris_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0b, 0x00);
}

uint8_t tb_iris_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t iris_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x4b, iris_value);
}


/* AE */
uint8_t tb_ae_auto(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x39, 0x00);
}

uint8_t tb_ae_manual(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x39, 0x03);
}


/* WB */
uint8_t tb_wb_auto(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x35, 0x00);
}

uint8_t tb_wb_manual(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x35, 0x05);
}

uint8_t tb_wb_one_push(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x10, 0x05);
}


/* GAIN */
uint8_t tb_gain_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0c, 0x02);
}

uint8_t tb_gain_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0c, 0x03);
}

uint8_t tb_gain_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0c, 0x00);
}

uint8_t tb_gain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t gain_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x4c, gain_value);
}


/* RGAIN */
uint8_t tb_rgain_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x03, 0x02);
}

uint8_t tb_rgain_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x03, 0x03);
}

uint8_t tb_rgain_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x03, 0x00);
}

uint8_t tb_rgain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t rgain_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x43, rgain_value);
}


/* BGAIN */
uint8_t tb_bgain_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x04, 0x02);
}

uint8_t tb_bgain_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x04, 0x03);
}

uint8_t tb_bgain_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x04, 0x00);
}

uint8_t tb_bgain_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bgain_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x44, bgain_value);
}


/* BRIGHT EXP */
uint8_t tb_bright_exp(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x3e, en);
}

uint8_t tb_bright_exp_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0e, 0x02);
}

uint8_t tb_bright_exp_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0e, 0x03);
}

uint8_t tb_bright_exp_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0e, 0x00);
}

uint8_t tb_bright_exp_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bright_exp_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x4e, bright_exp_value);
}


/* BRIGHT */
uint8_t tb_bright_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0d, 0x02);
}

uint8_t tb_bright_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0d, 0x03);
}

uint8_t tb_bright_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0d, 0x00);
}

uint8_t tb_bright_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t bright_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x4d, bright_value);
}


/* SHUTTER */
uint8_t tb_shutter_up(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0a, 0x02);
}

uint8_t tb_shutter_down(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0a, 0x03);
}

uint8_t tb_shutter_reset(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x0a, 0x00);
}

uint8_t tb_shutter_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t shutter_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x4a, shutter_value);
}


/* BACKLIGHT */
uint8_t tb_backlight(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x33, en);
}


///////////////////
/* PTZF COMMANDS */
///////////////////

/* ZOOM-FOCUS */
static inline uint8_t tb_zoom(struct tb_if *interface, uint8_t cam_addr, uint8_t arg1)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x07, arg1);
}

uint8_t tb_zoom_tele(struct tb_if *interface, uint8_t cam_addr, uint8_t zoom_speed)
{
	return tb_zoom(interface, cam_addr, 0x20 | (0x0f & zoom_speed));
}

uint8_t tb_zoom_tele_std(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_zoom(interface, cam_addr, 0x02);
}

uint8_t tb_zoom_wide(struct tb_if *interface, uint8_t cam_addr, uint8_t zoom_speed)
{
	return tb_zoom(interface, cam_addr, 0x30 | (0x0f & zoom_speed));
}

uint8_t tb_zoom_wide_std(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_zoom(interface, cam_addr, 0x03);
}

uint8_t tb_zoom_stop(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_zoom(interface, cam_addr, 0x00);
}

uint8_t tb_zoom_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t zoom_position)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x47, zoom_position);
}

uint8_t tb_dzoom(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_feature_enable(interface, cam_addr, 0x04, 0x06, en);
}

uint8_t tb_zoomfocus_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t zoom_position, uint16_t focus_position)
{
	return tb_2_16_value_set(interface, cam_addr, 0x04, 0x47, zoom_position, focus_position);
}

uint8_t tb_focus_auto(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x38, 0x02);
}

uint8_t tb_focus_manual(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x38, 0x03);
}

static inline uint8_t tb_focus(struct tb_if *interface, uint8_t cam_addr, uint8_t arg1)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x08, arg1);
}

uint8_t tb_focus_far(struct tb_if *interface, uint8_t cam_addr, uint8_t focus_speed)
{
	return tb_focus(interface, cam_addr, 0x20 | (0x0f & focus_speed));
}

uint8_t tb_focus_far_std(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_focus(interface, cam_addr, 0x02);
}

uint8_t tb_focus_near(struct tb_if *interface, uint8_t cam_addr, uint8_t focus_speed)
{
	return tb_focus(interface, cam_addr, 0x30 | (0x0f & focus_speed));
}

uint8_t tb_focus_near_std(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_focus(interface, cam_addr, 0x03);
}

uint8_t tb_focus_stop(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_focus(interface, cam_addr, 0x00);
}

uint8_t tb_focus_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t focus_position)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x48, focus_position);
}


/* PAN-TILT */
uint8_t tb_pt(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint8_t pan_dir, uint8_t tilt_dir)
{
	/* pan_dir: 1 left, 2 right, 3 none */
	/* tilt_dir: 1 up, 2 down, 3 none */
	INIT_COMMAND(0x06, 0x01, (TB_PT_SPD_MSK & pan_speed), (TB_PT_SPD_MSK & tilt_speed), pan_dir, tilt_dir);
	return SEND_COMMAND();
}

uint8_t tb_pt_up(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x03, 0x01);
}

uint8_t tb_pt_down(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x03, 0x02);
}

uint8_t tb_pt_left(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x01, 0x03);
}

uint8_t tb_pt_right(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x02, 0x03);
}

uint8_t tb_pt_upleft(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x01, 0x01);
}

uint8_t tb_pt_upright(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x02, 0x01);
}

uint8_t tb_pt_downleft(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x01, 0x02);
}

uint8_t tb_pt_downright(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed)
{
	return tb_pt(interface, cam_addr, pan_speed, tilt_speed, 0x02, 0x02);
}

uint8_t tb_pt_stop(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_pt(interface, cam_addr, 3, 3, 0x03, 0x03);
}

uint8_t tb_pt_absolute(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint16_t pan_position, uint16_t tilt_position)
{
	INIT_COMMAND(0x06, 0x02, (TB_PT_SPD_MSK & pan_speed), (TB_PT_SPD_MSK & tilt_speed), SPLIT16(pan_position), SPLIT16(tilt_position));
	return SEND_COMMAND();
}

uint8_t tb_pt_relative(struct tb_if *interface, uint8_t cam_addr, uint8_t pan_speed, uint8_t tilt_speed, uint16_t pan_position, uint16_t tilt_position)
{
	INIT_COMMAND(0x06, 0x03, (TB_PT_SPD_MSK & pan_speed), (TB_PT_SPD_MSK & tilt_speed), SPLIT16(pan_position), SPLIT16(tilt_position));
	return SEND_COMMAND();
}

uint8_t tb_pt_home(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x06, 0x04);
	return SEND_COMMAND();
}

uint8_t tb_pt_reset(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x06, 0x05);
	return SEND_COMMAND();
}

uint8_t tb_pt_limit_upright(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position)
{
	INIT_COMMAND(0x06, 0x07, 0x00, 0x01, SPLIT16(pan_position), SPLIT16(tilt_position));
	return SEND_COMMAND();
}

uint8_t tb_pt_limit_downleft(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position)
{
	INIT_COMMAND(0x06, 0x07, 0x00, 0x00, SPLIT16(pan_position), SPLIT16(tilt_position));
	return SEND_COMMAND();
}

uint8_t tb_pt_limit_upright_clear(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x06, 0x07, 0x01, 0x01);
	return SEND_COMMAND();
}

uint8_t tb_pt_limit_downleft_clear(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x06, 0x07, 0x01, 0x00);
	return SEND_COMMAND();
}

///////////////
/* INQUIRIES */
///////////////
uint8_t tb_cam_id_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *cam_id)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x22, cam_id);
}

uint8_t tb_power_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *power_status)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x00, power_status);
}

uint8_t tb_mirror_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *mirror_status)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x61, mirror_status);
}

uint8_t tb_flip_status_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *flip_status)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x06, flip_status);
}

uint8_t tb_ir_output_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *ir_output_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x06, 0x08, ir_output_mode);
}

uint8_t tb_iris_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *iris_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x4B, iris_value);
}

uint8_t tb_ae_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *ae_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x39, ae_mode);
}

uint8_t tb_wb_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *wb_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x35, wb_mode);
}

uint8_t tb_gain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *gain_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x4C, gain_value);
}

uint8_t tb_rgain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *rgain_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x43, rgain_value);
}

uint8_t tb_bgain_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bgain_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x44, bgain_value);
}

uint8_t tb_bright_exp_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *bright_exp_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x3E, bright_exp_mode);
}

uint8_t tb_bright_exp_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bright_exp_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x4E, bright_exp_value);
}

uint8_t tb_bright_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *bright_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x4D, bright_value);
}

uint8_t tb_shutter_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *shutter_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x4A, shutter_value);
}

uint8_t tb_backlight_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *backlight_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x33, backlight_mode);
}

uint8_t tb_dzoom_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *dzoom_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x06, dzoom_mode);
}

uint8_t tb_zoom_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *zoom_position)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x47, zoom_position);
}

uint8_t tb_focus_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *focus_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x38, focus_mode);
}

uint8_t tb_focus_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *focus_position)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x48, focus_position);
}

uint8_t tb_pt_pos_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *pan_position, uint16_t *tilt_position)
{
	return tb_2_16_inq(interface, cam_addr, 0x06, 0x12, pan_position, tilt_position);
}

uint8_t tb_video_format_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *video_format)
{
	return tb_1_16_inq(interface, cam_addr, 0x06, 0x23, video_format);
}
