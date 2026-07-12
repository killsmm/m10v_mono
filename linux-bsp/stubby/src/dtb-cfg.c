/*
 * (C) Copyright 2014 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include "asm/utils.h"
#include "mmc.h"
#include "fat.h"
#include "block.h"
#include "io.h"
#include "string.h"
#include "crypto/crypto.h"
#include "device.h"

#if defined(CONFIG_FILEPATH_DTB_CONFIG)

char append_file[4096]  __attribute__ (( section (".sdram_data") ));

static int to_hex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	
	return -1;
}

void process_dtb_config(void *dtb, char *dtb_config, int dtb_config_len)
{
	int b, n, m, len, curr_len;
	char c;
	char path[256];
	char payload[512];
	int payload_length;
	void *param;
	enum parser_state {
		PS_SOL,
		PS_COMMENT_OR_PATH,
		PS_WHITESPACE,
		PS_PLUS_EQUAL,
		PS_EQUAL,
		PS_WHITESPACE2,
		PS_WS_QUOTE_BRACK,
		PS_HEXBYTES,
		PS_HEXBYTES2,
		PS_WORDS,
		PS_x,
		PS_STRING_QUOTE,
		PS_HEX,
		PS_DEC,
		PS_WHITESPACE_COMMA,
		PS_IGNORE
	};
	enum parser_state ps = PS_SOL;
	int plus, string;

	n = 0;
	while (n < dtb_config_len) {
		c = dtb_config[n];
		switch (ps) {
		case PS_SOL:
			payload_length = 0;
			m = 0;
			plus = 0;
			string = 0;
			ps = PS_COMMENT_OR_PATH;
			/* fallthru */
		case PS_COMMENT_OR_PATH:
			if (!m && c == '#') {
				ps = PS_IGNORE;
				break;
			}
			if (m >= sizeof(path) - 2) {
				ps = PS_IGNORE;
				break;
			}
			if (c == ' ' || c == '\t')
				break;

			if (c == '+' || c == '=') {
				param = fdt_find_path(dtb, path, &curr_len);
				if (!param) {
					puts("  ");
					puts(path);
					puts(" - unknown path\n");
					ps = PS_IGNORE;
					break;
				}
			}
			
			if (c == '+') {
				plus = 1;
				ps = PS_EQUAL;
				break;
			}
			if (c == '=') {
				ps = PS_WS_QUOTE_BRACK;
				break;
			}
			path[m++] = c;
			path[m] = '\0';
			break;
		case PS_WHITESPACE:
			if (c == '=') {
				ps = PS_WS_QUOTE_BRACK;
				break;
			}
			if (c == '+') {
				plus = 1;
				ps = PS_EQUAL;
				break;
			}
			if (c != ' ' && c != '\t')
				ps = PS_IGNORE;
			break;
		case PS_EQUAL:
			if (c == '=') {
				ps = PS_WS_QUOTE_BRACK;
				break;
			}
			ps = PS_IGNORE;
			break;
		case PS_WS_QUOTE_BRACK:
			if (c == ' ' || c == '\t')
				break;

			if (c == '\"') {
				ps = PS_STRING_QUOTE;
				break;
			}
			if (c == '[') {
				ps = PS_HEXBYTES;
				break;
			}
			if (c == '<') {
				b = 0;
				ps = PS_WORDS;
				break;
			}
			ps = PS_IGNORE;
			break;

		/* [ XX XX ... ] hex byte array */

		case PS_HEXBYTES:
			if (c == ' ' || c == '\t')
				break;
			if (c == ']')
				goto write_payload;
			b = to_hex(c);
			if (b >= 0) {
				ps = PS_HEXBYTES2;
				break;
			}
			ps = PS_IGNORE;
			break;
		case PS_HEXBYTES2:
			c = to_hex(c);
			if (c < 0) {
				ps = PS_IGNORE;
				break;
			}
			if (payload_length > sizeof(payload) - 2) {
				ps = PS_IGNORE;
				break;
			}
			payload[payload_length++] = (b << 4) | c;
			ps = PS_HEXBYTES;
			break;
			
		/* < 123 0x123 ... > */
			
		case PS_WORDS:
			if (c == ' ' || c == '\t')
				break;
			if (c == '>')
				goto write_payload;
			if (c == '0') {
				ps = PS_x;
				break;
			}
			if (c >= '1' && c <= '9') {
				b = c - '0';
				ps = PS_DEC;
				break;
			}
			ps = PS_IGNORE;
			break;

		case PS_x:
			if (c == 'x') {
				ps = PS_HEX;
				break;
			}
			ps = PS_IGNORE;
			break;

		case PS_STRING_QUOTE:
			string = 1;
			if (c == '\"')
				goto write_payload;
			if (payload_length > sizeof(payload) - 2) {
				ps = PS_IGNORE;
				break;
			}
			payload[payload_length++] = c;
			break;

		case PS_DEC:
			if (payload_length > sizeof(payload) - 6) {
				ps = PS_IGNORE;
				break;
			}
			if (c == ' ' || c == '\t') {
				uaw_be32(&payload[payload_length], b);
				payload_length += 4;
				ps = PS_WORDS;
				break;
			}
			if (c == '>') {
				uaw_be32(&payload[payload_length], b);
				payload_length += 4;
				goto write_payload;
			}
			if (c >= '0' && c <= '9') {
				b = (b * 10) + (c - '0');
				break;
			}
			ps = PS_IGNORE;
			break;

		case PS_HEX:
			if (payload_length > sizeof(payload) - 6) {
				ps = PS_IGNORE;
				break;
			}
			if (c == ' ' || c == '\t') {
				uaw_be32(&payload[payload_length], b);
				payload_length += 4;
				ps = PS_WORDS;
				break;
			}
			if (c == '>') {
				uaw_be32(&payload[payload_length], b);
				payload_length += 4;
				goto write_payload;
			}
			c = to_hex(c);
			if (c >= 0) {
				b = (b << 4) + c;
				break;
			}
			ps = PS_IGNORE;
			break;

		case PS_IGNORE:
			if (c == '\n')
				ps = PS_SOL;
			break;
write_payload:
			ps = PS_IGNORE;
			payload[payload_length] = '\0';
			puts("  ");
			puts(path);
			if (plus) {
				puts(" += ");
				fdt_set_prop_size(dtb, param, curr_len + payload_length);
				if (string) {
					puts("\"");
					puts(payload);
					puts("\"");
					if (curr_len)
						curr_len--;
					payload_length++;
				} else {
					printdec(payload_length);
					puts(" bytes");
				}
				puts("\n");
				memcpy(param + curr_len, payload, payload_length);
			} else {
				puts(" = ");
				if (string) {
					puts("\"");
					puts(payload);
					puts("\"");
					payload_length++;
				} else {
					printdec(payload_length);
					puts(" bytes");
				}
				puts("\n");
				fdt_set_prop_size(dtb, param, payload_length);
				memcpy(param, payload, payload_length);
			}
			break;
		}
		n++;
	}
}
#endif
