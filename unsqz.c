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

// private headers (in alphabetical order)
#include "bitstream.h"
#include "debug.h"
#include "exitcodes.h"
#include "ucmdline.h"

// public headers (in alphabetical order)
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	parse_cmdline(argc, argv);

	bitstream* bs = bitstream_construct_from_file(cmdline_inputfilename);

	switch(cmdline_compressed_format) {
		case FORMAT_QS1:
			break;
		default:
			fprintf(stderr, FL "no handler for format %d\n", (int)cmdline_compressed_format);
			exit(EXIT_IMPLEMENTATION);
	}
	return EXIT_SUCCESS;
}
