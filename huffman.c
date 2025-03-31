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

#include "huffman_internal.h"

#include "debug.h"
#include "exitcodes.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

huffman* huffman_construct() {
	huffman* that = calloc(1, sizeof(huffman));
	if (!that) {
		fprintf(stderr, FL "Can't allocate huffman structure (%zu bytes)\n", sizeof (huffman));
		exit(EXIT_MEMORY);
	}
	that -> input_symbol_min = LONG_MAX;
	that -> input_symbol_max = LONG_MIN;
	return that;
}

void huffman_destruct(huffman *const that) {
	if (that) {
		free(that -> symbols);
	}
	free(that);
}

void huffman_compute_symbol_range(huffman *const that,
			long const *const source_symbols,
			long const source_size) {
	if (!that) {
		fprintf(stderr, FL "Computing Huffman symbol range on NULL object\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (that -> input_symbol_min <= that -> input_symbol_max) {
		fprintf(stderr, FL "Huffman symbol range already computed\n");
		exit(EXIT_INVALIDSTATE);
	}
	for (long i = 0; i < source_size; i++) {
		if (source_symbols[i] < that -> input_symbol_min) {
			that -> input_symbol_min = source_symbols[i];
		}
		if (source_symbols[i] > that -> input_symbol_max) {
			that -> input_symbol_max = source_symbols[i];
		}
	}
	if (verbosity >= VERB_EXTRA) {
		printf("Huffman symbol range: %ld-%ld\n", that -> input_symbol_min, that -> input_symbol_max);
	}
}

void huffman_compute_symbol_counts(huffman *const that,
			long const *const source_symbols,
			long const source_size) {
	if (!that) {
		fprintf(stderr, FL "Computing Huffman symbol counts on NULL object\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (that -> input_symbol_min > that -> input_symbol_max) {
		fprintf(stderr, FL "Computing Huffman symbol counts on processor without symbol ranges\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (that -> symbols) {
		fprintf(stderr, FL "Computing Huffman symbol counts a second time\n");
		exit(EXIT_INVALIDSTATE);
	}
	that -> symbols = calloc(that -> input_symbol_max - that -> input_symbol_min + 1, sizeof(hsymbol));
	if (!that -> symbols) {
		fprintf(stderr, FL "Can't allocate Huffman symbols (%ld times %zu bytes)\n",
					that -> input_symbol_max - that -> input_symbol_min + 1,
					sizeof(hsymbol));
		exit(EXIT_MEMORY);
	}
	for (long i = 0; i < source_size; i++) {
		that -> symbols[source_symbols[i] - that -> input_symbol_min].count++;
	}
	if (verbosity >= VERB_EXTRA) {
		for (long i = 0; i <= that -> input_symbol_max - that -> input_symbol_min; i++) {
			printf("Huffman symbol count: %ld instances of %ld\n",
					that -> symbols[i].count,
					i + that -> input_symbol_min);
		}
	}
}

void huffman_count_symbols_present(huffman *const that) {
	if (!that) {
		fprintf(stderr, FL "Counting Huffman symbols present on NULL object\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (!that -> symbols) {
		fprintf(stderr, FL "Computing Huffman symbols present on processor without symbol counts\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (that -> symbols_present) {
		fprintf(stderr, FL "Counting Huffman symbols present a second time\n");
		exit(EXIT_INVALIDSTATE);
	}
	for (long i = 0; i <= that -> input_symbol_max - that -> input_symbol_min; i++) {
		if (that -> symbols[i].count > 0) {
			that -> symbols_present++;
		}
	}
	if (verbosity >= VERB_EXTRA) {
		printf("Huffman %ld symbols present\n", that -> symbols_present);
	}
}

void huffman_build_tree(huffman* const that) {
	if (!that) {
		fprintf(stderr, FL "Building Huffman tree on NULL object\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (!that -> symbols_present) {
		fprintf(stderr, FL "Building Huffman tree on processor without symbols present\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (that -> tree) {
		fprintf(stderr, FL "Building Huffman tree a second time\n");
		exit(EXIT_INVALIDSTATE);
	}

	that -> tree = calloc(2 * that -> symbols_present - 1, sizeof(hnode));
	if (!that -> tree) {
		fprintf(stderr, FL "Can't allocate Huffman tree (%ld times %zu bytes)\n",
					2 * that -> symbols_present - 1,
					sizeof(hnode));
		exit(EXIT_MEMORY);
	}
	for (long i = 0; i < 2 * that -> symbols_present - 1; i++) {
		that -> tree[i].child0= LONG_MAX;
		that -> tree[i].child1 = LONG_MAX;
		that -> tree[i].parent = LONG_MAX;
	}
	long j = 0;
	for (long i = 0; i <= that -> input_symbol_max - that -> input_symbol_min; i++) {
		if (that -> symbols[i].count != 0) {
			that -> tree[j].value = i + that -> input_symbol_min;
			that -> tree[j].count = that -> symbols[i].count;
			j++;
		}
	}

	// Huffman core algorithm
	for (long i = that -> symbols_present ; i < 2 * that -> symbols_present - 1; i++) {
		long mi1, mi2, mv1, mv2;
		mi1 = mi2 = mv1 = mv2 = LONG_MAX;
		for (long j = 0; j < i; j++) {
			if (that -> tree[j].parent != LONG_MAX) {
				continue;
			}
			if (mv1 > that -> tree[j].count) {
				mv2 = mv1;
				mi2 = mi1;
				mv1 = that -> tree[j].count;
				mi1 = j;
			} else if (mv2 > that -> tree[j].count) {
				mv2 = that -> tree[j].count;
				mi2 = j;
			}
		}
		if (verbosity >= VERB_EXTRA) {
			printf("Huffman core min values %ld %ld at %ld %ld\n", mv1, mv2, mi1, mi2);
		}
		that -> tree[i].child0 = mi1;
		that -> tree[i].child1 = mi2;
		that -> tree[i].count = mv1 + mv2;
		that -> tree[mi1].parent = i;
		that -> tree[mi2].parent = i;
	}

	// Display Huffman tree
	if (verbosity >= VERB_EXTRA) {
		for (long i = 0; i < 2 * that -> symbols_present - 1; i++) {
			printf("Huffman node %ld", i);
			if (that -> tree[i].child0 == LONG_MAX) {
				printf(" value %ld", that -> tree[i].value);
			} else {
				printf(" children %ld %ld", that -> tree[i].child0, that -> tree[i].child1);
			}
			printf(" count %ld\n", that -> tree[i].count);
		}
	}
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

bitstream* encode_huffman(long* source, long ssize) {

	huffman* hf = huffman_construct();
	huffman_compute_symbol_range(hf, source, ssize);

	// Compute range of symbols. mn = minimum, mx = maximum.
	long mn = hf -> input_symbol_min, mx = hf -> input_symbol_max;
	printf("min %ld max %ld\n", mn, mx);

	// Gather counts of symbols
	struct huffsymbol* symbols = malloc((mx - mn + 1) * sizeof(struct huffsymbol));
	memset(symbols, 0, (mx - mn + 1) * sizeof(struct huffsymbol));

	huffman_compute_symbol_counts(hf, source, ssize);
	for (long i = 0; i <= mx - mn; i++) {
		symbols[i].count = hf -> symbols[i].count;
	}
	for (long i = 0; i <= mx - mn; i++) {
		printf("raw counts: value %ld count %ld\n", mn + i, symbols[i].count);
	}

	// Count unique symbols
	long nz = 0;
	huffman_count_symbols_present(hf);
	nz = hf -> symbols_present;
	printf("%ld unique symbols\n", nz);

	huffman_build_tree(hf);

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

	long t = mx - mn;
	long bs = 0;
	while (t) {
		bs++;
		t >>=1;
	}
	t = 2 * (nz - 1);
	long bn = 0;
	while (t) {
		bn++;
		t >>=1;
	}

//	printf("space needed:\n");
//	printf("%ld leaf nodes covering a span of %ld symbols (%ld bits): %ld bits total\n", nz, mx - mn + 1, bs, nz * bs);
//	printf("%ld inner nodes with %ld nodes to choose from (%ld bits): %ld bits total\n", nz - 1, 2 * (nz - 1), 2 * bn, (nz - 1) * 2 * bn);

	printf("%ld nodes, %ld bits each, %ld bits total\n", nz -1, 2 * (bs + 1), (nz - 1) * 2 * (bs + 1));

	// 3 bits = add 3, read that many bits after implicit MSB, subtract 5, result = number of used symbols
	// 000xxx = 3 extra bits, 8-15 -> 3-10
	// 001xxxx = 4 extra bits, 16-31 -> 11-26
	// 010xxxxx = 5 extra bits, 32-63 -> 27-58
	// 011xxxxxx = 6 extra bits, 64-127 -> 59-122
	// 100xxxxxxx = 7 extra bits, 128-255 -> 123-250
	// 101xxxxxxxx = 9 extra bits, 256-511 -> 251-506
	// 110xxxxxxxxx = 9 extra bits, 512-1023 -> 507-1018
	// 111xxxxxxxxxx = 10 extra bits, 1024-2047 -> 1019-2042
	// 2042 is enough for a 2D signed delta with enough additional symbols for LZ, RLE, BWT

	// store span of symbols (can assume there are at least as many as used symbols)



	// store symbol offset

	bitstream* stream = bitstream_construct();

	for (long i = nz; i < 2 * nz - 1; i++) {
		printf("node %ld - child 0 %ld child 1 %ld\n", i, tree[i].child0, tree[i].child1);
	}

	return stream;
}
