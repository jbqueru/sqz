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

#define SS(x) SSS(x)
#define SSS(x) #x
#define FL __FILE__ ":" SS(__LINE__) ": "

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long* read_pi1();
long* encode_huffman(long* source);

int main(int /* argc */, char** /* argv */) {
	long* pixels = NULL;
	pixels = read_pi1();
	if (!pixels) {
		fprintf(stderr, FL "Couldn't read pixels\n");
	}
	encode_huffman(pixels);
	free(pixels);
	return 0;
}

long* read_pi1() {
	char* rawbits = NULL;
	long* pixels = NULL;
	FILE* file = NULL;

	rawbits = malloc(32000);
	if (!rawbits) {
		fprintf(stderr, FL "Couldn't allocate memory for raw PI1 bits.\n");
		exit(1);
	}

	file = fopen("out/tos/MBVMAX.PI1", "rb");
	if (!file) {
		fprintf(stderr, FL "Couldn't open file for reading.\n");
		exit(1);
	}
	if (fseek(file, 34, SEEK_SET)) {
		fprintf(stderr, FL "Couldn't seek to pixel data in input file.\n");
		exit(1);
	}
	if (fread(rawbits, 1, 32000, file) < 32000) {
		fprintf(stderr, FL "Couldn't read pixel data from input file.\n");
		exit(1);
	}
	if (fclose(file)) {
		fprintf(stderr, FL "Couldn't close input file.\n");
		exit(1);
	}
	file = NULL;

	pixels = malloc(64000 * sizeof(long));
	if (!pixels) {
		fprintf(stderr, FL "Couldn't allocate memory for decoded pixels.\n");
		exit(1);
	}

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
	rawbits = NULL;

	return pixels;
}

long* encode_huffman(long* source) {

	// Compute range of symbols. mn = minimum, mx = maximum.
	long mn = LONG_MAX, mx = LONG_MIN;
	for (long i = 0; i < 64000; i++) {
		if (source[i] < mn) mn = source[i];
		if (source[i] > mx) mx = source[i];
	}
	printf("min %ld max %ld\n", mn, mx);

	// Gather counts of symbols
	long* rawcounts = malloc((mx - mn + 1) * sizeof(long));
	memset(rawcounts, 0, (mx - mn + 1) * sizeof(long));

	for (long i = 0; i < 64000; i++) {
		rawcounts[source[i] - mn]++;
	}
	for (long i = 0; i <= mx - mn; i++) {
		printf("raw counts: value %ld count %ld\n", mn + i, rawcounts[i]);
	}

	// Count unique symbols
	long nz = 0;
	for (long i = 0; i <= mx - mn; i++) {
		if (rawcounts[i]) nz++;
	}
	printf("%ld unique symbols\n", nz);

	long* counts = rawcounts;
	long* values = malloc((mx - mn + 1) * sizeof(long));
	for (long i = 0; i <= mx - mn; i++) {
		values[mn + i] = i;
	}
	for (long n = 0; n < mx - mn; n++) {
		for (long i = 0; i < mx - mn; i++) {
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
