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

#include "bitstream.h"
#include "cmdline.h"
#include "debug.h"
#include "exitcodes.h"
#include "huffman.h"
#include "image.h"
#include "lz78.h"

#include "other_formats/degas.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
	parse_cmdline(argc, argv);
	struct image* img = read_pi1(cmdline_inputfilename);

	long* pixels = malloc(64000 * sizeof(long));
	for (int i = 0; i < 64000; i++) {
		pixels[i] = img->pixels[i];
	}

	lz78encoder* lz78 = lz78encoder_construct();
	lz78encoder_compute_symbol_range(lz78, pixels, 64000);
	lz78encoder_find_matches(lz78, pixels, 64000);
	lz78encoder_destruct(lz78);

/*
	huffman* h = huffman_construct();
	huffman_compute_symbol_range(h, pixels, 64000);
	huffman_compute_symbol_counts(h, pixels, 64000);
	huffman_count_symbols_present(h);
	huffman_build_tree(h);
	huffman_build_codes(h);

	bitstream* s = bitstream_construct();

	huffman_write_tree(h, s);
	if (cmdline_outputfilename) {
		bitstream_dump_to_file(s, cmdline_outputfilename);
	}

	huffman_destruct(h);
*/
	free(pixels);
	return EXIT_SUCCESS;
}
