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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long* read_pi1();

int main(int /* argc */, char** /* argv */) {
	long* pixels = read_pi1();
	free(pixels);
	return 0;
}

long* read_pi1() {
	char* rawbits = malloc(32000);
	long* pixels = malloc(64000 * sizeof(long));
	FILE* infile = fopen("out/tos/MBVMAX.PI1", "rb");
	fseek(infile, SEEK_SET, 34);
	fread(rawbits, 1, 32000, infile);
	for (int y = 0; y < 200; y++) {
		for (int x = 0; x < 320; x++) {
			long c = 0;
			for (int b = 0; b < 4; b++) {
				if (rawbits[(x / 16) * 8 + (x / 8) + 160 * y + b * 2] & (0x80 >> (x % 8))) {
					c += (1 << b);
				}
			}
			pixels[x + 320 * y] = c;
		}
	}
	free(rawbits);
	return pixels;
}
