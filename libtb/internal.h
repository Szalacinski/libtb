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
#ifndef __LIBTB_INTERNAL_H__
#define __LIBTB_INTERNAL_H__
#include <libtb/libtb.h>
/* Use this header when defining any custom commands for your own cameras. */

/* A common (but still ugly) hack to count the number of arguments. */

#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N
#define PP_RSEQ_N() 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

/* 
 * Sets up a command with the arguments passed.
 * The fact that it counts the number of arguments 
 * helps prevent programmer errors.
 * We actually have to give the array a name,
 * due to standard C++ not supporting compound literals.
 */
#define INIT_PACKET(...) uint8_t __arr[PP_NARG(__VA_ARGS__) + 2] = {0x00, __VA_ARGS__, 0xFF};\
                         uint8_t __read_arr[TB_MAX_PACKET] = { 0 };

#define INIT_INQUIRY(...) INIT_PACKET(0x09, __VA_ARGS__)
#define INIT_COMMAND(...) INIT_PACKET(0x01, __VA_ARGS__)

/* Sends a regular command */
#define SEND_COMMAND() tb_send_command_get_reply(interface, cam_addr, __arr, sizeof(__arr), __read_arr)
#define SEND_BROADCAST() tb_send_command_get_reply(interface, 8,  __arr, sizeof(__arr), __read_arr)

#define SPLIT8(x) ((x & 0xF0) >>  4), (x & 0x0F)
#define SPLIT12(x) ((x & 0x0F00) >>  8), SPLIT8(x)
#define SPLIT16(x) ((x & 0xF000) >> 12), SPLIT12(x)

#define ENABLE(x) (x ? 0x02 : 0x03)

/* !!! Make sure that resp_len <= TB_MAX_PACKET !!! */
#define HANDLE_INQUIRY(resp_len, resp_handler, ...) INIT_INQUIRY(__VA_ARGS__);\
                                         uint8_t err = SEND_COMMAND();\
                                         if (!err) { resp_handler; }\
                                         else if (__read_arr[resp_len - 1] != 0xFF) { err = TB_ERROR_UNEXPECTED_PACKET; }\
                                         return err

/* Response handlers for HANDLE_INQUIRY() */
#define TB_1_4_BIT_RESP(x) *x = (__read_arr[2] & 0x0F)
#define TB_1_16_BIT_RESP(x) *x = ((__read_arr[2] & 0x0F) << 12) | ((__read_arr[3] & 0x0F) << 8) | ((__read_arr[4] & 0x0F) << 4) | (__read_arr[5] & 0x0F)
#define TB_2_16_BIT_RESP(x, y) *x = ((__read_arr[2] & 0x0F) << 12) | ((__read_arr[3] & 0x0F) << 8) | ((__read_arr[4] & 0x0F) << 4) | (__read_arr[5] & 0x0F);\
                               *y = ((__read_arr[6] & 0x0F) << 12) | ((__read_arr[7] & 0x0F) << 8) | ((__read_arr[8] & 0x0F) << 4) | (__read_arr[9] & 0x0F)
#define TB_1_32_BIT_RESP(x) *x = ((__read_arr[2] & 0x0F) << 28) | ((__read_arr[3] & 0x0F) << 24) | ((__read_arr[4] & 0x0F) << 20) | ((__read_arr[5] & 0x0F) << 16) |\
                                 ((__read_arr[6] & 0x0F) << 12) | ((__read_arr[7] & 0x0F) << 8) | ((__read_arr[8] & 0x0F) << 4) | (__read_arr[9] & 0x0F)
#define TB_PT_SPD_MSK 0x1f

#ifdef __cplusplus
extern "C" {
#endif

uint8_t tb_send_command_get_reply(struct tb_if *interface, uint8_t cam_addr, uint8_t *arr, uint8_t arr_size, uint8_t *read_arr);
uint8_t tb_cmd(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint8_t cmd3);
uint8_t tb_feature_enable(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, bool en);
uint8_t tb_1_16_value_set(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t value);
uint8_t tb_2_16_value_set(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t value1, uint16_t value2);

uint8_t tb_1_4_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint8_t *resp);
uint8_t tb_1_16_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t *resp);
uint8_t tb_2_16_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t *resp1, uint16_t *resp2);
uint8_t tb_1_32_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint32_t *resp);

#ifdef __cplusplus
}
#endif

#endif /* __LIBTB_INTERNAL_H__ */
