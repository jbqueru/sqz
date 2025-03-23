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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long* read_pi1();
long* encode_huffman(long* source);

int main(int /* argc */, char** /* argv */) {
	long* pixels = read_pi1();
	encode_huffman(pixels);
	free(pixels);
	return 0;
}

long* read_pi1() {
	char* rawbits = malloc(32000);
	long* pixels = malloc(64000 * sizeof(long));
	FILE* infile = fopen("out/tos/MBVMAX.PI1", "rb");
	fseek(infile, SEEK_SET, 34);
	fread(rawbits, 1, 32000, infile);
	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			long c = 0;
			for (int b = 0; b < 4; b++) {
				if (rawbits[(x / 16) * 8 + (x / 8) + 160 * y + b * 2] & (0x80 >> (x % 8))) {
					c += (1 << b);
				}
			}
			pixels[x + 320 * y] = c;
		}
	}
	free(rawbits);
	return pixels;
}

long* encode_huffman(long* source) {
	long mn = LONG_MAX, mx = LONG_MIN;
	for (int i = 0; i < 64000; i++) {
		if (source[i] < mn) mn = source[i];
		if (source[i] > mx) mx = source[i];
	}
	printf("min %ld max %ld\n", mn, mx);
	long* counts = malloc((mx - mn + 1) * sizeof(long));
	long* values = malloc((mx - mn + 1) * sizeof(long));
	for (long i = 0; i <= mx - mn; i++) {
		counts[mn + i] = 0;
		values[mn + i] = i;
	}
	for (int i = 0; i < 64000; i++) {
		counts[source[i]]++;
	}
	for (long i = 0; i <= mx - mn; i++) {
		printf("before sorting: value %ld count %ld\n", values[mn + i], counts[mn + i]);
	}
	for (int n = 0; n < mx - mn; n++) {
		for (int i = 0; i < mx - mn; i++) {
			if (counts[i] < counts[i + 1]) {
				long t = counts[i + 1];
				counts[i + 1] = counts[i];
				counts[i] = t;
				t = values[i + 1];
				values[i + 1] = values[i];
				values[i] = t;
			}
		}
	}
	for (long i = 0; i <= mx - mn; i++) {
		printf("after sorting: value %ld count %ld\n", values[mn + i], counts[mn + i]);
	}
	return NULL;
}
