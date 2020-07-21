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
#include <libserialport.h>
#include <libtb/protocols/serial.h>


int8_t tb_serial_connect(struct tb_if *i, char *name)
{
	struct sp_port **port = (struct sp_port**)&(i->connection_info);
	enum sp_return ret;
	
	if ((ret = sp_get_port_by_name(name, port)) != SP_OK) {
		return (int8_t)ret;
	}

	if ((ret = sp_open(*port, SP_MODE_READ_WRITE)) != SP_OK) {
		sp_free_port(*port);
		return (int8_t)ret;
	}
	
	if ((ret = sp_set_baudrate(*port, 9600)) != SP_OK ||
		(ret = sp_set_bits(*port, 8)) != SP_OK ||
		(ret = sp_set_parity(*port, SP_PARITY_NONE)) != SP_OK ||
		(ret = sp_set_stopbits(*port, 1)) != SP_OK ||
		(ret = sp_set_xon_xoff(*port, SP_XONXOFF_DISABLED)) != SP_OK ||
		(ret = sp_set_flowcontrol(*port, SP_FLOWCONTROL_NONE)) != SP_OK) {
		
		/* Error opening or configuring serial port */
		sp_close(*port);
		sp_free_port(*port);
	}
	
	return (int8_t)ret;
}

int8_t tb_serial_disconnect(struct tb_if *i)
{
		struct sp_port *port = (struct sp_port*)i->connection_info;
		enum sp_return ret = SP_OK;
		
		if (port) {
			ret = sp_close(port);
			sp_free_port(port);
		}
		return (int8_t)ret;
}

int8_t tb_serial_speed_change(struct tb_if *i, int baudrate)
{
	struct sp_port **port = (struct sp_port**)&(i->connection_info);
	return sp_set_baudrate(*port, baudrate);
}

int tb_serial_write(void *port, uint8_t *buf, uint8_t count)
{
	return sp_nonblocking_write((struct sp_port*)port, buf, count);
}

int tb_serial_read(void *port, uint8_t *buf, uint8_t count)
{
	return sp_blocking_read((struct sp_port*)port, buf, count, 5000);	
}
