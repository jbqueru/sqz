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

#include "bitstream.h"
#include "debug.h"
#include "exitcodes.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

const size_t bitstream_increment = 10240;

struct bitstream {
	size_t size;
	size_t current;

	unsigned char* array;
	size_t allocated;
};

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

void bitstream_write(bitstream *const that, int bit) {
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
