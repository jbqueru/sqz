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

#include "lz78_internal.h"

#include "debug.h"
#include "exitcodes.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

lz78encoder* lz78encoder_construct() {
	lz78encoder* that = calloc(1, sizeof(lz78encoder));
	if (!that) {
		fprintf(stderr, FL "Can't allocate lz78encoder structure (%zu bytes)\n", sizeof (lz78encoder));
		exit(EXIT_MEMORY);
	}
	that -> input_symbol_min = LONG_MAX;
	that -> input_symbol_max = LONG_MIN;
	return that;
}

void lz78encoder_destruct(lz78encoder *const that) {
	free(that);
}

void lz78encoder_compute_symbol_range(
			lz78encoder *const that,
			long const *const symbols,
			long const symbol_count) {
	for (long i = 0; i < symbol_count; i++) {
		if (symbols[i] < that -> input_symbol_min) {
			that -> input_symbol_min = symbols[i];
		}
		if (symbols[i] > that -> input_symbol_max) {
			that -> input_symbol_max = symbols[i];
		}
	}
	printf("min symbol %ld max symbol %ld\n",
				that -> input_symbol_min,
				that -> input_symbol_max);
}

void lz78encoder_find_matches(
			lz78encoder *const that,
			long const *const symbols,
			long const symbol_count) {
	lz78trie *const root = lz78encoder_construct_trie(that);
	long next_node = 3;
	for (long i = 0; i < symbol_count; i++) {
		lz78trie* search = root;
		printf("looking for match for symbol %ld at offset %ld\n", symbols[i], i);
		while (i < symbol_count && search -> next_level[symbols[i] - that -> input_symbol_min]) {
			printf("found partial match for offset %ld\n", i);
			search = search -> next_level[symbols[i] - that -> input_symbol_min];
			i++;
		}
		printf("outputing node %ld\n", search -> node_id);

		printf("outputing literal %ld at offset %ld\n", (i < symbol_count) ? symbols[i] : - 1, i);

		printf("creating node %ld\n", next_node);
		lz78trie *const next = lz78encoder_construct_trie(that);
		next -> node_id = next_node;
		search -> next_level[symbols[i] - that -> input_symbol_min] = next;
		next_node++;
	}
}

void lz78encoder_output_entry(lz78encoder* const that, long const node_id, long const symbol) {
	that -> stream_length++;
	that -> stream_nodes = realloc(that -> stream_nodes, that -> stream_length * sizeof (long));
	that -> stream_nodes[that -> stream_length - 1] = node_id;
	that -> stream_symbols = realloc(that -> stream_symbols, that -> stream_length * sizeof (long));
	that -> stream_symbols[that -> stream_length - 1] = symbol;
}

lz78trie* lz78encoder_construct_trie(lz78encoder *const that) {
	lz78trie* ret = calloc(1, sizeof(lz78trie) + (that -> input_symbol_max - that -> input_symbol_min) * sizeof(lz78trie*));
	return ret;
}
