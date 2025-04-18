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

#include <stdio.h>
#include <stdlib.h>

void process_qs1(bitstream *const stream) {
	int palette_present[16];
	unsigned char palette_red[16], palette_green[16], palette_blue[16];
	int bit;

	for (int i = 0; i < 16; i++) {
		bit = bitstream_read_bit(stream);
		if (bit < 0) {
			fprintf(stderr, "invalid QS1 file, missing palette information\n");
			exit(EXIT_BADFILE);
		}
		palette_present[i] = bit;
		if (verbosity >= VERB_EXTRA) {
			printf("QS1: color %d %spresent", i, bit ? "" : "not ");
		}
		if (i == 0 || bit) {
			long c;
			c = bitstream_read_value(stream, 3);
			if (c < 0) {
				fprintf(stderr, "invalid QS1 file, missing palette information\n");
				exit(EXIT_BADFILE);
			}
			palette_red[i] = c;
			c = bitstream_read_value(stream, 3);
			if (c < 0) {
				fprintf(stderr, "invalid QS1 file, missing palette information\n");
				exit(EXIT_BADFILE);
			}
			palette_green[i] = c;
			c = bitstream_read_value(stream, 3);
			if (c < 0) {
				fprintf(stderr, "invalid QS1 file, missing palette information\n");
				exit(EXIT_BADFILE);
			}
			palette_blue[i] = c;
			if (verbosity >= VERB_EXTRA) {
				printf(" value %d%d%d\n", palette_red[i], palette_green[i], palette_blue[i]);
			}
		} else {
			palette_red[i] = palette_green[i] = palette_blue[i] = 0;
			if (verbosity >= VERB_EXTRA) {
				printf("\n");
			}
		}
	}
	long numcolors = 0;
	for (int i = 0; i < 16; i++) {
		if (palette_present[i]) {
			numcolors++;
		}
	}
	if (verbosity >= VERB_EXTRA) {
		printf("QS1: image has %ld colors\n", numcolors);
	}
	bit = bitstream_read_bit(stream);
	if (bit < 0) {
		fprintf(stderr, "invalid QS1 file, missing LZ2 type information\n");
		exit(EXIT_BADFILE);
	}
	if (verbosity >= VERB_EXTRA) {
		if (bit == 0) {
			printf("QS1: LZ78 format (alternate back-references and literal symbols)\n");
		} else {
			printf("QS1: LZW format (pre-initialized dictionary, only back-references)\n");
		}
	}
	int fulldict = bitstream_read_value(stream, 2);
	if (fulldict < 0) {
		fprintf(stderr, "invalid QS1 file, missing LZ2 full-dictionary information\n");
		exit(EXIT_BADFILE);
	}
	if (fulldict >= 3) {
		bit = bitstream_read_bit(stream);
		if (bit < 0) {
			fprintf(stderr, "invalid QS1 file, incomplete LZ2 full-dictionary information\n");
			exit(EXIT_BADFILE);
		}
		fulldict += bit;
	}
	if (verbosity >= VERB_EXTRA) {
		switch (fulldict) {
			case 0:
				printf("QS1: remove LRU entry on full dictionary\n");
				break;
			case 1:
				printf("QS1: remove one leaf entry, round-robin\n");
				break;
			case 2:
				printf("QS1: remove all leaf entries\n");
				break;
			case 3:
				printf("QS1: clear entire dictionary\n");
				break;
			case 4:
				printf("QS1: keep dictionary full (explicit clear command)\n");
				break;
		}
	}
}
