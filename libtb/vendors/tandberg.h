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
#ifndef __LIBTB_VENDOR_TANDBERG_H__
#define __LIBTB_VENDOR_TANDBERG_H__
#include <libtb/libtb.h>

#ifdef __cplusplus
extern "C" {
#endif

/* REBOOT */
uint8_t tb_tandberg_boot(struct tb_if *interface, uint8_t cam_addr);

/* LED */
uint8_t tb_tandberg_power_led(struct tb_if *interface, uint8_t cam_addr, bool en);
uint8_t tb_tandberg_call_led(struct tb_if *interface, uint8_t cam_addr, bool en);
uint8_t tb_tandberg_call_led_blink(struct tb_if *interface, uint8_t cam_addr);

/* WB */
uint8_t tb_tandberg_wb_table_manual(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_tandberg_wb_table_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t table);

/* GAMMA */
uint8_t tb_tandberg_gamma_auto(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_tandberg_gamma_manual(struct tb_if *interface, uint8_t cam_addr);
uint8_t tb_tandberg_gamma_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t gamma_value);

/* MOTOR MOVEMENT DETECT */
uint8_t tb_tandberg_mm_detect(struct tb_if *interface, uint8_t cam_addr, bool en);

/* IR */
uint8_t tb_tandberg_ir_camera_control(struct tb_if *interface, uint8_t cam_addr, bool en);

/* PAN-TILT-ZOOM-FOCUS */
/* !!!DO NOT ROUTE THIS COMMAND THROUGH NON-TANDBERG CAMERAS!!! */
uint8_t tb_tandberg_ptzf_direct(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position, uint16_t zoom_position, uint16_t focus_position);
//This one is for the Wave II and PrecisionHD 720p
/* !!!DO NOT ROUTE THIS COMMAND THROUGH NON-TANDBERG CAMERAS!!! */
uint8_t tb_tandberg_ptzf_direct_720p(struct tb_if *interface, uint8_t cam_addr, uint16_t pan_position, uint16_t tilt_position, uint16_t zoom_position, uint16_t focus_position);

/* SERIAL SPEED */
//Wait 20 seconds after the response before sending commands on the interface after sending this command.
//Be sure to change the interface's serial speed to 115200 at high, and 9600 at low.
uint8_t tb_tandberg_cam_serial_speed(struct tb_if *interface, uint8_t cam_addr, bool high);

/* VIDEO FORMAT */
uint8_t tb_tandberg_video_format(struct tb_if *interface, uint8_t cam_addr, uint8_t format);


///////////////
/* INQUIRIES */
///////////////

uint8_t tb_tandberg_als_rgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *rgain_value);
uint8_t tb_tandberg_als_bgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *bgain_value);
uint8_t tb_tandberg_als_ggain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *ggain_value);
uint8_t tb_tandberg_als_wgain_inq(struct tb_if *interface, uint8_t cam_addr, uint32_t *wgain_value);

uint8_t tb_tandberg_dip_switch_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *dip_switch_value);
uint8_t tb_tandberg_upside_down_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *upside_down);
uint8_t tb_tandberg_gamma_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *gamma_mode);
uint8_t tb_tandberg_gamma_table_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *gamma_table);
uint8_t tb_tandberg_wb_table_inq(struct tb_if *interface, uint8_t cam_addr, uint16_t *wb_table);
uint8_t tb_tandberg_call_led_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *call_led_mode);
uint8_t tb_tandberg_pwr_led_mode_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t *pwr_led_mode);

#ifdef __cplusplus
}
#endif
#endif /* __LIBTB_VENDOR_TANDBERG_H__ */
