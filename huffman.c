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
			that -> symbols[i].node = j;
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

void huffman_build_codes(huffman* const that) {
	if (!that) {
		fprintf(stderr, FL "Building Huffman codes on NULL object\n");
		exit(EXIT_INVALIDSTATE);
	}
	if (!that -> tree) {
		fprintf(stderr, FL "Building Huffman tree on processor without tree\n");
		exit(EXIT_INVALIDSTATE);
	}
	for (long i = 0; i < that -> symbols_present; i++) {
		for (long j = i; that -> tree[j].parent != LONG_MAX; j = that -> tree[j].parent) {
			that -> symbols[that -> tree[i].value - that -> input_symbol_min].bits++;
		}
	}

	if (verbosity >= VERB_EXTRA) {
		for (long i = 0; i <= that -> input_symbol_max - that -> input_symbol_min; i++) {
			printf("Huffman symbol value %ld ", i);
			if (that -> symbols[i].count) {
				printf("node %ld code ", that -> symbols[i].node);
				for (long j = that -> symbols[i].bits - 1; j >= 0; j--) {
					long n = that -> symbols[i].node;
					for (long k = 0; k < j; k++) {
						n = that -> tree[n].parent;
					}
					if (that -> tree[that -> tree[n].parent].child0 == n) {
						printf("0");
					} else {
						printf("1");
					}
				}
				printf(" (%ld bits)", that -> symbols[i].bits);
			} else {
				printf("not present in input");
			}
			printf("\n");
		}
	}
}

/*
 * Assume that the original symbols are at most 8 bit.
 *
 * Worst pre-processing (2D signed delta-packing) will make the range
 * 		-510..510, i.e. 0..1020 after offset.
 * The last few values might be used for meta data, e.g. BTW markers,
 *      bijective numeration, EOF marker.
 * The system will limit itself to 1025 symbols numbered 0..1024, meaning
 * 		1024 nodes numbered 1025..2048, where the root is 2048 and the
 * 		inner nodes are 1025-2047, i.e. node entries are expected to fit
 * 		in 11 bits.
 * At the bottom end, the smallest sensical Huffman tree has 3 symbols, 2
 *		nodes, 1 inner node, i.e. 2 bits per node entry.
 * It would be theoretically possible to limit the range from 4 to 11 bits
 * 		per node entry, at the cost of a little efficiency for very
 * 		small trees (8->16, 18->24 and 24->32 bits respectively for 3, 4,
 * 		and 5 symbols), but that leaves no room to store variants of
 * 		canonicalized trees and supporting those would immediately
 * 		require an extra bit somewhere that would negate the benefits.
 * 		By storing all possible values from 2 to 11, we have 6 values
 * 		left for extensions into canonicalized trees.
 */

/*
 * Node count can range from 2 to 1024.
 * 		000xxx (8-15) -> 2-9
 * 		001xxxx (16-31) -> 10-25
 * 		010xxxxx (32-63) -> 26-57
 * 		...
 * 		111xxxxxxxxxx (1024-2047) -> 1018-2041
 * Note: there should be ways to use smaller storage for node counts
 * 		when it's known that the symbol range is shorter.
 */

/* Write Huffman tree */
void huffman_write_tree(huffman *const that, bitstream *const stream) {
	if (verbosity >= VERB_EXTRA) {
		printf("Huffman processor for %ld nodes\n", that -> symbols_present - 1);
		printf("Huffman processor for %ld symbols\n", that -> input_symbol_max - that -> input_symbol_min + 1);
		long t = that -> input_symbol_max - that -> input_symbol_min + that -> symbols_present - 1;
		printf("Huffman nodes have %ld possible values\n", t);
		long bits = 0;
		while (t) {
			bits++;
			t >>= 1;
		}
		printf("Huffman nodes need %ld bits per entry, %ld bits per node\n", bits, 2 * bits);
		printf("Huffman tree needs %ld bits\n", 2 * bits * (that -> symbols_present - 1));
		printf("\n");
		printf("Huffman node entry bit size %ld\n", bits);
		printf("Huffman node count %ld\n", that -> symbols_present - 1);
		printf("Huffman symbol offset %ld\n", that -> input_symbol_min);
	}
}
