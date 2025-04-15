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

#ifndef PALETTE_H_INCLUDED
#define PALETTE_H_INCLUDED

typedef enum {
	PALETTE_RGB = 0,
	PALETTE_RGB_EHB = 1,
	PALETTE_RGB_27,
	PALETTE_SINCLAIR_QL4,
	PALETTE_SINCLAIR_ZX_SPECTRUM,
	PALETTE_CGA,
	PALETTE_CGA_CLONE,
	PALETTE_CGA_4_1LOW,
	PALETTE_CGA_4_1HIGH,
	PALETTE_CGA_4_2LOW,
	PALETTE_CGA_4_2HIGH,
	PALETTE_CGA_4_3LOW,
	PALETTE_CGA_5HIGH,
	PALETTE_CGA_5LOW,
	PALETTE_CGA_6,
	PALETTE_MO5,
	PALETTE_TO770_1,
	PALETTE_TO770_2,
	PALETTE_TO770_3,
	PALETTE_ATARI_8BIT,
	PALETTE_APPLEII_LOW,
	PALETTE_APPLEII_HIGH,
	PALETTE_VIC20,
	PALETTE_C64,
	PALETTE_CPLUS4,
	PALETTE_TMS9918,
	PALETTE_MC6847,
} palette_type;

// VIC20-LOW?
// SAM COUPE?

typedef struct palette {
	int type;
	int num_colors;
	int bit_depth;
	unsigned char red[256];
	unsigned char green[256];
	unsigned char blue[256];
} palette;

void process_qs1(bitstream *const stream);

#endif /* PALETTE_H_INCLUDED */
