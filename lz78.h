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
 * Definitions for a Lempel-Ziv-78 processor
 */

#ifndef LZ78_H_INCLUDED
#define LZ78_H_INCLUDED

#include "bitstream.h"

typedef struct lz78encoder lz78encoder;

lz78encoder* lz78encoder_construct();

void lz78encoder_destruct(lz78encoder *const that);

void lz78encoder_compute_symbol_range(
    lz78encoder *const that,
    long const *const symbols,
    long const symbol_count);

void lz78encoder_find_matches(
    lz78encoder *const that,
    long const *const symbols,
    long const symbol_count);

#endif /* LZ78_H_INCLUDED */
