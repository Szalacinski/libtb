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
#include <libtb/vendors/tandberg.h>
#include <libtb/internal.h>

/* REBOOT */
uint8_t tb_tandberg_boot(struct tb_if *interface, uint8_t cam_addr)
{
	INIT_COMMAND(0x42);
	return SEND_COMMAND();
}

/* LED */
uint8_t tb_tandberg_power_led(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_cmd(interface, cam_addr, 0x33, 0x02, (uint8_t)en);
}

uint8_t tb_tandberg_call_led(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_cmd(interface, cam_addr, 0x33, 0x01, (uint8_t)en);
}

uint8_t tb_tandberg_call_led_blink(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x33, 0x01, 0x02);
}

/* WB */
uint8_t tb_tandberg_wb_table_manual(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x35, 0x06);
}

uint8_t tb_tandberg_wb_table_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t table)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x75, table);
}

/* GAMMA */
uint8_t tb_tandberg_gamma_auto(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x51, 0x02);
}

uint8_t tb_tandberg_gamma_manual(struct tb_if *interface, uint8_t cam_addr)
{
	return tb_cmd(interface, cam_addr, 0x04, 0x51, 0x03);
}

uint8_t tb_tandberg_gamma_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t gamma_value)
{
	return tb_1_16_value_set(interface, cam_addr, 0x04, 0x52, gamma_value);
}

/* MOTOR MOVEMENT DETECT */
uint8_t tb_tandberg_mm_detect(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_cmd(interface, cam_addr, 0x50, 0x30, (uint8_t)en);
}

/* IR */
uint8_t tb_tandberg_ir_camera_control(struct tb_if *interface, uint8_t cam_addr, bool en)
{
	return tb_cmd(interface, cam_addr, 0x06, 0x09, ENABLE(en));
}

/* PAN-TILT-ZOOM-FOCUS */
/* !!!DO NOT ROUTE THIS COMMAND THROUGH NON-TANDBERG CAMERAS!!! */
uint8_t tb_tandberg_ptzf_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position, uint16_t zoom_position, uint16_t focus_position)
{
	INIT_COMMAND(0x06, 0x20, SPLIT16(pan_position), SPLIT16(tilt_position), SPLIT16(zoom_position), SPLIT16(focus_position));
	return SEND_COMMAND();
}

/* !!!DO NOT ROUTE THIS COMMAND THROUGH NON-TANDBERG CAMERAS!!! */
uint8_t tb_tandberg_ptzf_direct_720p(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position, uint16_t zoom_position, uint16_t focus_position)
{
	INIT_COMMAND(0x37, SPLIT12(pan_position), SPLIT8(tilt_position), SPLIT12(zoom_position), SPLIT16(focus_position));
	return SEND_COMMAND();
}

/* SERIAL SPEED */
uint8_t tb_tandberg_cam_serial_speed(struct tb_if *interface, uint8_t cam_addr, bool high)
{
	INIT_COMMAND(0x34, (uint8_t)high);
	return SEND_COMMAND();
}

/* VIDEO FORMAT */
uint8_t tb_tandberg_video_format(struct tb_if *interface, uint8_t cam_addr, uint8_t format)
{
	INIT_COMMAND(0x35, 0x00, (format & 0x0F), 0x00);
	return SEND_COMMAND();
}

///////////////
/* INQUIRIES */
///////////////

static uint8_t tb_tandberg_custom_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint8_t cmd3, uint8_t *resp)
{
	HANDLE_INQUIRY(4, TB_1_4_BIT_RESP(resp), cmd1, cmd2, cmd3);
}

uint8_t tb_tandberg_als_rgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *rgain_value)
{
	return tb_1_32_inq(interface, cam_addr, 0x50, 0x50, rgain_value);
}

uint8_t tb_tandberg_als_bgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *bgain_value)
{
	return tb_1_32_inq(interface, cam_addr, 0x50, 0x51, bgain_value);
}

uint8_t tb_tandberg_als_ggain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *ggain_value)
{
	return tb_1_32_inq(interface, cam_addr, 0x50, 0x52, ggain_value);
}

uint8_t tb_tandberg_als_wgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *wgain_value)
{
	return tb_1_32_inq(interface, cam_addr, 0x50, 0x53, wgain_value);
}

uint8_t tb_tandberg_dip_switch_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *dip_switch_value)
{
	return tb_1_16_inq(interface, cam_addr, 0x06, 0x24, dip_switch_value);
}

uint8_t tb_tandberg_upside_down_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *upside_down)
{
	return tb_1_4_inq(interface, cam_addr, 0x50, 0x70, upside_down);
}

uint8_t tb_tandberg_gamma_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *gamma_mode)
{
	return tb_1_4_inq(interface, cam_addr, 0x04, 0x51, gamma_mode);
}

uint8_t tb_tandberg_gamma_table_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *gamma_table)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x52, gamma_table);
}

uint8_t tb_tandberg_wb_table_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *wb_table)
{
	return tb_1_16_inq(interface, cam_addr, 0x04, 0x75, wb_table);
}

uint8_t tb_tandberg_call_led_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *call_led_mode)
{
	return tb_tandberg_custom_inq(interface, cam_addr, 0x01, 0x33, 0x01, call_led_mode);
}

uint8_t tb_tandberg_pwr_led_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *pwr_led_mode)
{
	return tb_tandberg_custom_inq(interface, cam_addr, 0x01, 0x33, 0x01, pwr_led_mode);
}
