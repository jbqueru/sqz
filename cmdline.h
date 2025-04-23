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

#ifndef CMDLINE_H_INCLUDED
#define CMDLINE_H_INCLUDED

enum delta {
	DELTA_UNSPECIFIED = 0,
    DELTA_ANY,
    DELTA_NONE,
    DELTA_ARITHMETIC_1D,
    DELTA_ARITHMETIC_2D,
    DELTA_WRAP_1D,
    DELTA_WRAP_2D,
    DELTA_XOR_1D,
    DELTA_XOR_2D,
};

enum pixel_order {
	ORDER_UNSPECIFIED = 0,
	ORDER_ANY,
	ORDER_HORIZONTAL_PIXEL,
	ORDER_VERTICAL_PIXEL,
	ORDER_HILBERT_PIXEL,
	ORDER_HORIZONTAL_CHARACTER,
	ORDER_VERTICAL_CHARACTER,
	ORDER_HILBERT_CHARACTER,
};

enum transform {
	TRANSFORM_UNSPECIFIED = 0,
	TRANSFORM_ANY,
	TRANSFORM_NONE,
	TRANSFORM_BURROWS_WHEELER,
	TRANSFORM_MOVE_TO_FRONT,
	TRANSFORM_DELTA_ARITHMETIC,
	TRANSFORM_DELTA_WRAP,
	TRANSFORM_DELTA_XOR,
};

enum compression {
	COMPRESSION_UNSPECIFIED = 0,
	COMPRESSION_ANY,
	COMPRESSION_NONE,
	COMPRESSION_LZ78,
	COMPRESSION_HUFFMAN,
};

extern char* cmdline_inputfilename;
extern char* cmdline_outputfilename;
extern enum delta cmdline_delta;
extern enum pixel_order cmdline_pixelorder;
extern enum transform cmdline_transform[10];
extern enum compression cmdline_compression[10];

void parse_cmdline(int argc, char** argv);
void display_version();
void display_help(char const *const progname);

#endif /* CMDLINE_H_INCLUDED */
