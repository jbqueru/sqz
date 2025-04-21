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

#include "image.h"

#include <stdlib.h>
#include <stdio.h>

void image_destruct(struct image *const that) {
	if (that) {
		free(that -> pixels);
	}
	free(that);
}

void image_log(struct image const *const that) {
	printf("Image size %dx%d, ", that -> width, that -> height);
	switch(that -> par) {
		case PAR_SQUARE:
			printf("square pixels");
			break;
		case PAR_WIDE:
			printf("wide pixels");
			break;
		case PAR_TALL:
			printf("tall pixels");
			break;
		case PAR_EXTRA_WIDE:
			printf("extra-wide pixels");
			break;
		case PAR_EXTRA_TALL:
			printf("extra-tall pixels");
			break;
	}
	printf(", %d bits/pixel\n", that -> bpp);
	printf("Color space: ");
	switch(that -> palette) {
		case PAL_RGB3:
			printf("RGB 3:3:3 (512 colors)");
			break;
		default:
			printf("unknown palette");
	}
	printf(", ");
	switch(that -> lookup) {
		case LOOKUP_FULL:
			printf("full palette lookup");
			break;
		case LOOKUP_FIXED:
			printf("direct color addressing");
			break;
		default:
			printf("unknown lookup");
	}
	printf("\n");
	if (that -> separate_border) {
		printf("Border has a separate entry");
	} else {
		printf("Border is part of regular palette");
	}
	printf("\n");
	switch(that -> lookup) {
		case LOOKUP_FULL:
			for (int i = 0; i < (1 << that -> bpp); i++) {
				if ((i == 0 && !that -> separate_border) || that -> color_used[i]) {
					printf("Color %d: %02x %02x %02x", i, that -> red[i], that -> green[i], that -> blue[i]);
					if (that -> color_used[i]) {
						printf(", used in image");
					}
					printf("\n");
				}
			}
			break;
		default:
	}
}
