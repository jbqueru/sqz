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

#include "qs_internal.h"

#include "../bitstream.h"
#include "../debug.h"
#include "../exitcodes.h"
#include "../image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct image* read_qs1(bitstream *const stream) {
	struct image* ret = malloc(sizeof (struct image));
	memset(ret, 0, sizeof (struct image));
	ret -> width = 320;
	ret -> height = 200;
	ret -> par = PAR_SQUARE;
	ret -> bpp = 4;
	ret -> palette = PAL_RGB3;
	ret -> lookup = LOOKUP_FULL;
	ret -> separate_border = 0;

	for (int c = 0; c < 16; c++) {
		ret -> color_used[c] = bitstream_read_bit(stream);
	}
	for (int c = 0; c < 16; c++) {
		if (c == 0 || ret -> color_used[c]) {
			ret -> red[c] = bitstream_read_value(stream, 3);
			ret -> green[c] = bitstream_read_value(stream, 3);
			ret -> blue[c] = bitstream_read_value(stream, 3);
		}
	}
	ret -> pixels = malloc(64000);
	for (int i = 0; i < 64000; i++) {
		ret -> pixels[i] = bitstream_read_value(stream, 4);
	}
	return ret;
}

void write_qs1(struct image const *const img, bitstream *const stream) {
	if (img -> width != 320 || img -> height != 200 || img -> bpp != 4) {
		fprintf(stderr, "Error, QS1 files must be 320*200*4bpp\n");
		exit(EXIT_BADFILE);
	}
	if (img -> par != PAR_SQUARE) {
		printf("creating QS1 from image with non-square pixels\n");
	}
	if (img -> palette != PAL_RGB3 || img -> lookup != LOOKUP_FULL) {
		fprintf(stderr, "Error: QS1 files must be RGB3 full-lookup\n");
		exit(EXIT_BADFILE);
	}
	if (img -> separate_border) {
		printf("ignoring border color for QS1 image\n");
	}
	for (int c = 0; c < 16; c++) {
		bitstream_write_bit(stream, img -> color_used[c] != 0);
	}
	for (int c = 0; c < 16; c++) {
		if (c == 0 || img -> color_used[c]) {
			bitstream_write_value(stream, img -> red[c], 3);
			bitstream_write_value(stream, img -> green[c], 3);
			bitstream_write_value(stream, img -> blue[c], 3);
		}
	}
	for (int i = 0; i < 64000; i++) {
		bitstream_write_value(stream, img -> pixels[i], 4);
	}
}
