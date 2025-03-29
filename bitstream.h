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

#ifndef BITSTREAM_H_INCLUDED
#define BITSTREAM_H_INCLUDED

#include <stddef.h>

typedef struct bitstream bitstream;

bitstream* bitstream_construct();

void bitstream_destruct(bitstream *const that);

size_t bitstream_bit_size(bitstream *const that);

size_t bitstream_byte_size(bitstream *const that);

unsigned char const * bitstream_byte_array(bitstream *const that);

void bitstream_write(bitstream *const that, int bit);

#endif /* BITSTREAM_H_INCLUDED */
