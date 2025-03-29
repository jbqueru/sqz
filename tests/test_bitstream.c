/*
 * Copyright 2025 Jean-Baptiste M. "JBQ" "Djaybee" Queru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// SPDX-License-Identifier: AGPL-3.0-or-later

#include "../bitstream_internal.h"

#include <limits.h>
#include <stdio.h>

int test_init_state();
int test_write();

int main(int, char**) {
	if (CHAR_BIT != 8) {
		printf("tests not implemented for CHAR_BIT = %d\n", CHAR_BIT);
		return 1;
	}
	int ret = 0;
	ret |= test_init_state();
	ret |= test_write();
	return ret;
}

int test_init_state() {
	int ret = 0;
	bitstream* bs = bitstream_construct();
	if (bs -> size != 0) {
		printf("internal size after construct isn't zero\n");
		ret = 1;
	}
	if (bs -> current != 0) {
		printf("internal current after construct isn't zero\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 0) {
		printf("public bit_size after construct isn't zero\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 0) {
		printf("public byte_size after construct isn't zero\n");
		ret = 1;
	}
	bitstream_destruct(bs);
	return ret;
}

int test_write() {
	int ret = 0;
	bitstream* bs = bitstream_construct();
	bitstream_write(bs, 1);
	if (bs -> allocated == 0) {
		printf("allocated size zero after writing a bit\n");
		ret = 1;
	}
	if (bs -> array == NULL) {
		printf("private array missing after writing a bit\n");
		return 1;
	}
	if (bs -> size != 1) {
		printf("internal size after writing a bit isn't one\n");
		ret = 1;
	}
	if (bs -> current != 1) {
		printf("internal current after writing a bit isn't one\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 1) {
		printf("public bit_size after writing a bit isn't one\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 1) {
		printf("public byte_size after writing a bit isn't one\n");
		ret = 1;
	}
	if ((bs -> array[0] & 0x80) != 0x80) {
		printf("bit 0 not properly written (private)\n");
		ret = 1;
	}
	if ((bitstream_byte_array(bs)[0] & 0x80) != 0x80) {
		printf("bit 0 not properly written (public)\n");
		ret = 1;
	}
	bitstream_write(bs, 0);
	bitstream_write(bs, 1);
	bitstream_write(bs, 0);
	bitstream_write(bs, 0);
	bitstream_write(bs, 1);
	bitstream_write(bs, 0);
	bitstream_write(bs, 1);
	if (bs -> size != 8) {
		printf("internal size after writing 8 bits isn't 8\n");
		ret = 1;
	}
	if (bs -> current != 8) {
		printf("internal current after writing 8 bits isn't 8\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 8) {
		printf("public bit_size after writing 8 bits isn't 8\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 1) {
		printf("public byte_size after writing 8 bits isn't one\n");
		ret = 1;
	}
	if (bs -> array[0] != 0xa5) {
		printf("bits 0-7 not properly written #1 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[0] != 0xa5) {
		printf("bits 0-7 not properly written #1 (public)\n");
		ret = 1;
	}
	bitstream_write(bs, 1);
	bitstream_write(bs, 1);
	bitstream_write(bs, 1);
	bitstream_write(bs, 1);
	if (bs -> allocated < 2) {
		printf("allocated size too small after writing 12 bits\n");
		ret = 1;
	}
	if (bs -> size != 12) {
		printf("internal size after writing 12 bits isn't 12\n");
		ret = 1;
	}
	if (bs -> current != 12) {
		printf("internal current after writing 12 bits isn't 12\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 12) {
		printf("public bit_size after writing 12 bits isn't 12\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 2) {
		printf("public byte_size after writing 12 bits isn't 2\n");
		ret = 1;
	}
	if (bs -> array[0] != 0xa5) {
		printf("bits 0-7 not properly written #2 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[0] != 0xa5) {
		printf("bits 0-7 not properly written #2 (public)\n");
		ret = 1;
	}
	if ((bs -> array[1] & 0xf0) != 0xf0) {
		printf("bits 8-11 not properly written (private)\n");
		ret = 1;
	}
	if ((bitstream_byte_array(bs)[1] & 0xf0) != 0xf0) {
		printf("bits 8-11 not properly written (public)\n");
		ret = 1;
	}
	bitstream_write(bs, 0);
	bitstream_write(bs, 0);
	bitstream_write(bs, 0);
	bitstream_write(bs, 0);
	if (bs -> allocated < 2) {
		printf("allocated size too small after writing 16 bits\n");
		ret = 1;
	}
	if (bs -> size != 16) {
		printf("internal size after writing 16 bits isn't 16\n");
		ret = 1;
	}
	if (bs -> current != 16) {
		printf("internal current after writing 16 bits isn't 16\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 16) {
		printf("public bit_size after writing 16 bits isn't 16\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 2) {
		printf("public byte_size after writing 16 bits isn't 2\n");
		ret = 1;
	}
	if (bs -> array[0] != 0xa5) {
		printf("bits 0-7 not properly written #3 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[0] != 0xa5) {
		printf("bits 0-7 not properly written #3 (public)\n");
		ret = 1;
	}
	if (bs -> array[1] != 0xf0) {
		printf("bits 8-15 not properly written #1 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[1] != 0xf0) {
		printf("bits 8-15 not properly written #1 (public)\n");
		ret = 1;
	}
	bitstream_write(bs, 0);
	bitstream_write(bs, 0);
	if (bs -> allocated < 3) {
		printf("allocated size too small after writing 18 bits\n");
		ret = 1;
	}
	if (bs -> size != 18) {
		printf("internal size after writing 18 bits isn't 18\n");
		ret = 1;
	}
	if (bs -> current != 18) {
		printf("internal current after writing 18 bits isn't 18\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 18) {
		printf("public bit_size after writing 18 bits isn't 18\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 3) {
		printf("public byte_size after writing 18 bits isn't 3\n");
		ret = 1;
	}
	if (bs -> array[0] != 0xa5) {
		printf("bits 0-7 not properly written #4 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[0] != 0xa5) {
		printf("bits 0-7 not properly written #4 (public)\n");
		ret = 1;
	}
	if (bs -> array[1] != 0xf0) {
		printf("bits 8-15 not properly written #2 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[1] != 0xf0) {
		printf("bits 8-15 not properly written #2 (public)\n");
		ret = 1;
	}
	if ((bs -> array[2] & 0xc0) != 0x00) {
		printf("bits 16-17 not properly written (private)\n");
		ret = 1;
	}
	if ((bitstream_byte_array(bs)[2] & 0xc0) != 0x00) {
		printf("bits 16-17 not properly written (public)\n");
		ret = 1;
	}
	bitstream_write(bs, 1);
	bitstream_write(bs, 1);
	bitstream_write(bs, 1);
	bitstream_write(bs, 0);
	bitstream_write(bs, 1);
	bitstream_write(bs, 0);
	if (bs -> allocated < 3) {
		printf("allocated size too small after writing 24 bits\n");
		ret = 1;
	}
	if (bs -> size != 24) {
		printf("internal size after writing 24 bits isn't 24\n");
		ret = 1;
	}
	if (bs -> current != 24) {
		printf("internal current after writing 24 bits isn't 24\n");
		ret = 1;
	}
	if (bitstream_bit_size(bs) != 24) {
		printf("public bit_size after writing 24 bits isn't 24\n");
		ret = 1;
	}
	if (bitstream_byte_size(bs) != 3) {
		printf("public byte_size after writing 24 bits isn't 3\n");
		ret = 1;
	}
	if (bs -> array[0] != 0xa5) {
		printf("bits 0-7 not properly written #5 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[0] != 0xa5) {
		printf("bits 0-7 not properly written #5 (public)\n");
		ret = 1;
	}
	if (bs -> array[1] != 0xf0) {
		printf("bits 8-15 not properly written #3 (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[1] != 0xf0) {
		printf("bits 8-15 not properly written #3 (public)\n");
		ret = 1;
	}
	if (bs -> array[2] != 0x3a) {
		printf("bits 16-23 not properly written (private)\n");
		ret = 1;
	}
	if (bitstream_byte_array(bs)[2] != 0x3a) {
		printf("bits 16-23 not properly written (public)\n");
		ret = 1;
	}
	bitstream_destruct(bs);
	return ret;
}
