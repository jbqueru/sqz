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

#include "bitstream_internal.h"

#include "debug.h"
#include "exitcodes.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

const size_t bitstream_increment = 10240;

bitstream* bitstream_construct() {
	bitstream* that = (bitstream*) malloc(sizeof (bitstream));
	if (!that) {
		fprintf(stderr, FL "Can't allocate bitstream structure (%zu bytes)\n", sizeof (bitstream));
		exit(EXIT_MEMORY);
	}
	that -> size = 0;
	that -> current = 0;

	that -> array = NULL;
	that -> allocated = 0;
	return that;
}

bitstream* bitstream_construct_from_file(char const *const filename) {
	bitstream* that = (bitstream*) malloc(sizeof (bitstream));
	if (!that) {
		fprintf(stderr, FL "Can't allocate bitstream structure (%zu bytes)\n", sizeof (bitstream));
		exit(EXIT_MEMORY);
	}

	FILE* inputfile = fopen(filename, "rb");
	fseek(inputfile, 0, SEEK_END);
	long filesize = ftell(inputfile);
	fseek(inputfile, 0, SEEK_SET);

	that -> array = malloc(filesize);
	that -> allocated = filesize;
	if (fread(that -> array, 1, filesize, inputfile) < (size_t)filesize) {
		fprintf(stderr, FL "Can't read file %s\n", filename);
		exit (EXIT_INPUTFILE);
	}

	that -> size = filesize * CHAR_BIT;
	that -> current = 0;

	fclose(inputfile);
	return that;
}


void bitstream_destruct(bitstream *const that) {
	if (that -> array) {
		free(that -> array);
	}
	free(that);
}

size_t bitstream_bit_size(bitstream *const that) {
	return that -> size;
}

size_t bitstream_byte_size(bitstream *const that) {
	return (that -> size + CHAR_BIT - 1) / CHAR_BIT;
}

unsigned char const * bitstream_byte_array(bitstream *const that) {
	return that -> array;
}

int bitstream_read_bit(bitstream *const that) {
	if (that -> current == that -> size) {
		return -1;
	}
	size_t byte_offset = that -> current / CHAR_BIT;
	int ret = (that -> array[byte_offset] & (1U << (CHAR_BIT - 1 - that -> current % CHAR_BIT))) != 0;
	that -> current++;
	return ret;
}

long bitstream_read_value(bitstream *const that, int numbits) {
	long ret = 0;
	for (int i = 0; i < numbits; i++) {
		int bit = bitstream_read_bit(that);
		if (bit == -1) {
			return bit;
		}
		ret <<= 1;
		ret |= (bit != 0);
	}
	return ret;
}

void bitstream_write_bit(bitstream *const that, int bit) {
	if (that -> current + 1 > that -> size) {
		that -> size++;
		if (that -> size > that -> allocated * CHAR_BIT) {
			that -> allocated += bitstream_increment;
			that -> array = realloc(that -> array, that -> allocated);
			if (!that -> array) {
				fprintf(stderr, FL "Can't grow bitstream storage array (%zu bytes)\n", that -> allocated);
				exit(EXIT_MEMORY);
			}
		}
	}
	size_t byte_offset = that -> current / CHAR_BIT;
	unsigned char byte = 1U << (CHAR_BIT - 1 - that -> current % CHAR_BIT);
	if (bit) {
		that -> array[byte_offset] |= byte;
	} else {
		that -> array[byte_offset] &= ~byte;
	}
	that -> current++;
}

void bitstream_write_value(bitstream *const that, long value, int numbits) {
	for (int i = numbits - 1; i >= 0; i--) {
		bitstream_write_bit(that, (value & (1L << i)) != 0);
	}
}

void bitstream_dump_to_file(bitstream *const that, char const *const filename) {
	FILE* outputfile = fopen(filename, "wb");
	fwrite(bitstream_byte_array(that), 1, bitstream_byte_size(that), outputfile);
	fclose(outputfile);
}
