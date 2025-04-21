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

#include "degas.h"

#include "../debug.h"
#include "../exitcodes.h"
#include "../image.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct image* read_pi1(char* filename) {
	FILE* file = NULL;
	char* rawbits = NULL;
	struct image* ret = NULL;

	rawbits = malloc(32034);
	if (!rawbits) {
		fprintf(stderr, FL "Couldn't allocate memory for raw PI1 bits.\n");
		exit(EXIT_MEMORY);
	}
	memset(rawbits, 0, 32034);

	file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, FL "Couldn't open file for reading.\n");
		exit(EXIT_INPUTFILE);
	}
	if (fseek(file, 0, SEEK_END)) {
	    fprintf(stderr, FL "Couldn't seek to end of input file.\n");
	    exit(EXIT_INPUTFILE);
	}
	if (ftell(file) != 32034 && ftell(file) != 32068) {
		fprintf(stderr, "Invalid PI1 file: wrong size.\n");
		exit(EXIT_BADFILE);
	}
	if (fseek(file, 0, SEEK_SET)) {
		fprintf(stderr, FL "Couldn't seek back start of input file.\n");
		exit(EXIT_INPUTFILE);
	}

	if (fread(rawbits, 1, 32034, file) < 32034) {
		fprintf(stderr, FL "Couldn't read pixel data from input file.\n");
		exit(EXIT_INPUTFILE);
	}
	if (fclose(file)) {
		fprintf(stderr, FL "Couldn't close input file.\n");
		exit(EXIT_INPUTFILE);
	}
	file = NULL;

	if (rawbits[0] != 0 || rawbits[1] != 0) {
		fprintf(stderr, "Invalid PI1 file: wrong header.\n");
	}

	ret = malloc(sizeof(struct image));
	if (!ret) {
		fprintf(stderr, FL "Couldn't allocate memory for image structure.\n");
		exit(EXIT_MEMORY);
	}
	memset(ret, 0, sizeof(struct image));

	ret -> width = 320;
	ret -> height = 200;
	ret -> par = PAR_SQUARE;
	ret -> bpp = 4;
	ret -> lookup = LOOKUP_FULL;
	ret -> separate_border = 0;

	ret -> pixels = malloc(64000);
	if (!ret -> pixels) {
		fprintf(stderr, FL "Couldn't allocate memory for pixels.\n");
		exit(EXIT_MEMORY);
	}
	memset(ret -> pixels, 0, 64000);

	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			for (int b = 0; b < 4; b++) {
				if (rawbits[(x / 16) * 8 + (x % 16 / 8) + 160 * y + b * 2 + 34] & (0x80 >> (x % 8))) {
					ret -> pixels[x + 320 * y] |= (1 << b);
				}
			}
			if (verbosity >= VERB_EXTRA) {
				if (!ret -> color_used[ret -> pixels[x + 320 * y]]) {
					printf("Found pixel with color %d\n", ret -> pixels[x + 320 * y]);
				}
			}
			ret -> color_used[ret -> pixels[x + 320 * y]] = 1;
		}
	}

	ret -> palette = PAL_RGB3;
	for (int c = 0; c < 16; c++) {
		if (c != 0 && ret -> color_used[c] == 0) {
			continue;
		}
		if ((rawbits[2 * c + 2] & 0x08) || (rawbits[2 * c + 3] & 0x88)) {
			ret -> palette = PAL_RGB4;
		}
	}
	for (int c = 0; c < 16; c++) {
		if (c != 0 && ret -> color_used[c] == 0) {
			continue;
		}
		if (ret -> palette == PAL_RGB3) {
			ret -> red[c] = rawbits[2 * c + 2] & 0x07;
			ret -> green[c] = (rawbits[2 * c + 3] & 0x70) >> 4;
			ret -> blue[c] = rawbits[2 * c + 3] & 0x07;
		} else {
			ret -> red[c] = ((rawbits[2 * c + 2] & 0x07) << 1) | ((rawbits[2 * c + 2] & 0x08) >> 3);
			ret -> green[c] = ((rawbits[2 * c + 3] & 0x70) >> 3) | ((rawbits[2 * c + 3] & 0x80) >> 7);
			ret -> blue[c] = ((rawbits[2 * c + 3] & 0x07) << 1) | ((rawbits[2 * c + 3] & 0x08) >> 3);
		}
	}
	free(rawbits);
	rawbits = NULL;

	return ret;
}
