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

	for (int i = 0; i < 16; i++) {
		int bit;
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
}
