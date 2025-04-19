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

#include "palette.h"

#include <stdio.h>
#include <stdlib.h>

palette* palette_construct_from_qs1(bitstream *const stream) {
	palette* that;
	that = calloc(1, sizeof(palette));
	for (int i = 0; i < 16; i++) {
		that -> used[i] = bitstream_read_bit(stream);
		that -> present[i] = (that -> used[i] || (i == 0));
		if (i == 0 || that -> present[i]) {
			that -> red[i] = bitstream_read_value(stream, 3);
			that -> green[i] = bitstream_read_value(stream, 3);
			that -> blue[i] = bitstream_read_value(stream, 3);
		}
	}
	return that;
}

void palette_log(palette const *const that) {
	printf("Palette type: ");
	switch(that -> type) {
		case PALETTE_RGB:
			printf("RGB");
			break;
		default:
			printf("Unknown");
			break;
	}
	printf("\n");

	printf("Palette has %d colors", that -> num_colors);
	if (that -> type == PALETTE_RGB) {
		printf(", %d bits per component", that -> bit_depth);
	}
	printf("\n");

	for (int i = 0; i < that -> num_colors; i++) {
	}
}
