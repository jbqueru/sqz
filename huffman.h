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

/*
 * Definitions for a Huffman processor
 */

#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED

#include "bitstream.h"

typedef struct huffman huffman;

/* Construct a Huffman processor */
huffman* huffman_construct();

/* Destruct a Huffman processor */
void huffman_destruct(huffman *const that);

/* Compute range of input symbols */
void huffman_compute_symbol_range(huffman *const that,
			long const *const source_symbols,
			long const source_size);

/* Compute counts of input symbols */
void huffman_compute_symbol_counts(huffman *const that,
								   long const *const source_symbols,
								   long const source_size);

/* Count symbols present */
void huffman_count_symbols_present(huffman *const that);

/* Build Huffman tree */
void huffman_build_tree(huffman *const that);

/* Build Huffman codes */
void huffman_build_codes(huffman *const that);

#endif
