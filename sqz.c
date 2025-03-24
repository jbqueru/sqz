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
long* encode_huffman(long* source, long ssize);

int main(int /* argc */, char** /* argv */) {
	long* pixels = NULL;
	pixels = read_pi1();
	encode_huffman(pixels, 64000);
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
	memset(rawbits, 0, 32000);

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
	memset(pixels, 0, 64000 * sizeof(long));

	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			long c = 0;
			for (int b = 0; b < 4; b++) {
				if (rawbits[(x / 16) * 8 + (x % 16 / 8) + 160 * y + b * 2] & (0x80 >> (x % 8))) {
					c |= (1L << b);
				}
			}
			pixels[x + 320 * y] = c;
		}
	}

	free(rawbits);
	rawbits = NULL;

	return pixels;
}

struct huffsymbol {
	long count;
	long bits;
	long code;
};

struct hufftree {
	long value;
	long count;
	long child0;
	long child1;
	long parent;
};

long* encode_huffman(long* source, long ssize) {

	// Compute range of symbols. mn = minimum, mx = maximum.
	long mn = LONG_MAX, mx = LONG_MIN;
	for (long i = 0; i < ssize; i++) {
		if (source[i] < mn) mn = source[i];
		if (source[i] > mx) mx = source[i];
	}
	printf("min %ld max %ld\n", mn, mx);

	// Gather counts of symbols
	struct huffsymbol* symbols = malloc((mx - mn + 1) * sizeof(struct huffsymbol));
	memset(symbols, 0, (mx - mn + 1) * sizeof(struct huffsymbol));

	for (long i = 0; i < ssize; i++) {
		symbols[source[i] - mn].count++;
	}
	for (long i = 0; i <= mx - mn; i++) {
		printf("raw counts: value %ld count %ld\n", mn + i, symbols[i].count);
	}

	// Count unique symbols
	long nz = 0;
	for (long i = 0; i <= mx - mn; i++) {
		if (symbols[i].count > 0) {
			nz++;
		}
	}
	printf("%ld unique symbols\n", nz);

	// Prepare empty tree
	struct hufftree* tree = malloc((2 * nz - 1) * sizeof(struct hufftree));

	for (long i = 0; i < 2 * nz - 1; i++) {
		tree[i].child0= LONG_MAX;
		tree[i].child1 = LONG_MAX;
		tree[i].parent = LONG_MAX;
	}
	long j = 0;
	for (long i = 0; i <= mx - mn; i++) {
		if (symbols[i].count != 0) {
			tree[j].value = i + mn;
			tree[j].count = symbols[i].count;
			j++;
		}
	}

	// Huffman core algorithm
	for (long i = nz ; i < 2 * nz - 1; i++) {
		long mi1, mi2, mv1, mv2;
		mi1 = mi2 = mv1 = mv2 = LONG_MAX;
		for (long j = 0; j < i; j++) {
			if (tree[j].parent != LONG_MAX) {
				continue;
			}
			if (mv1 > tree[j].count) {
				mv2 = mv1;
				mi2 = mi1;
				mv1 = tree[j].count;
				mi1 = j;
			} else if (mv2 > tree[j].count) {
				mv2 = tree[j].count;
				mi2 = j;
			}
		}
		printf("min values %ld %ld at %ld %ld\n", mv1, mv2, mi1, mi2);
		tree[i].child0 = mi1;
		tree[i].child1 = mi2;
		tree[i].count = mv1 + mv2;
		tree[mi1].parent = i;
		tree[mi2].parent = i;
	}

	// Display Huffman tree
	for (long i = 0; i < 2 * nz - 1; i++) {
		printf("node %ld", i);
		if (tree[i].child0 == LONG_MAX) {
			printf(" value %ld", tree[i].value);
		} else {
			printf(" children %ld %ld", tree[i].child0, tree[i].child1);
		}
		printf(" count %ld\n", tree[i].count);
	}

	// Compute Huffman codes
	for (long i = 0; i < nz; i++) {
		for (long j = i; tree[j].parent != LONG_MAX; j = tree[j].parent) {
			if (tree[tree[j].parent].child1 == j) {
				symbols[tree[i].value - mn].code |= (1L << symbols[tree[i].value - mn].bits);
			}
			symbols[tree[i].value - mn].bits++;
		}
	}

	// Print Huffman codes
	for (long i = 0; i < nz; i++) {
		printf("symbol %ld code ", tree[i].value);
		for (long j = symbols[tree[i].value - mn].bits - 1; j >= 0; j--) {
			if (symbols[tree[i].value - mn].code & (1L << j)) {
				printf("1");
			} else {
				printf("0");
			}
		}
		printf(" (length %ld)\n", symbols[tree[i].value - mn].bits);
	}

	long tot = 0;
	for (long i = 0; i < nz; i++) {
		tot += symbols[tree[i].value - mn].bits * symbols[tree[i].value - mn].count;
	}
	printf("%ld total bits of Huffman codes (%ld bytes)\n", tot, (tot + 7) / 8);

	return NULL;
}
