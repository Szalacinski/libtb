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
#include <libtb/internal.h>

uint8_t tb_send_command_get_reply(struct tb_if *interface, uint8_t cam_addr, uint8_t *arr, uint8_t arr_size, uint8_t *read_arr)
{
	uint8_t tmp_addr = (0x0f & cam_addr);
	arr[0] = 0x80 | tmp_addr;
	int err = interface->write(interface->connection_info, arr, arr_size);
	if (err < arr_size) {
		return TB_ERROR_OTHER;
	}
	return interface->packet_wait((void*)interface, tmp_addr, read_arr);
}

uint8_t tb_cmd(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint8_t cmd3)
{
	INIT_COMMAND(cmd1, cmd2, cmd3);
	return SEND_COMMAND();
}

uint8_t tb_feature_enable(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, bool en)
{
	return tb_cmd(interface, cam_addr, cmd1, cmd2, ENABLE(en));
}

uint8_t tb_1_16_value_set(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t value)
{
	INIT_COMMAND(cmd1, cmd2, SPLIT16(value));
	return SEND_COMMAND();
}

uint8_t tb_2_16_value_set(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t value1, uint16_t value2)
{
	INIT_COMMAND(cmd1, cmd2, SPLIT16(value1), SPLIT16(value2));
	return SEND_COMMAND();
}

uint8_t tb_1_4_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint8_t *resp)
{
	HANDLE_INQUIRY(4, TB_1_4_BIT_RESP(resp), cmd1, cmd2);
}

uint8_t tb_1_16_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t *resp)
{
	HANDLE_INQUIRY(7, TB_1_16_BIT_RESP(resp), cmd1, cmd2);
}

uint8_t tb_2_16_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint16_t *resp1, uint16_t *resp2)
{
	HANDLE_INQUIRY(11, TB_2_16_BIT_RESP(resp1, resp2), cmd1, cmd2);
}

uint8_t tb_1_32_inq(struct tb_if *interface, uint8_t cam_addr, uint8_t cmd1, uint8_t cmd2, uint32_t *resp)
{
	HANDLE_INQUIRY(11, TB_1_32_BIT_RESP(resp), cmd1, cmd2);
}
