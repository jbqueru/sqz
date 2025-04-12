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

#ifndef LZ78_INTERNAL_H_INCLUDED
#define LZ78_INTERNAL_H_INCLUDED

#include "lz78.h"

struct lz78encoder {
    long input_symbol_min;
    long input_symbol_max;
};

typedef struct lz78trie {
    struct lz78trie* next_level[1];
} lz78trie;

lz78trie* lz78encoder_construct_trie(lz78encoder *const that);

#endif /* LZ78_INTERNAL_H_INCLUDED */
