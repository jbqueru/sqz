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

#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

enum pixel_aspect_ratio {
	PAR_SQUARE = 0,		// 0, square-ish pixels, typically 256-400 pixels per line
	PAR_WIDE,			// 10, wide pixels, typically 128-200 pixels per line
	PAR_TALL,			// 110, tall pixels, typically 512-800 pixels per line
	PAR_EXTRA_WIDE,		// 1110, extra-wide pixels, typically 64-100 pixels per line
	PAR_EXTRA_TALL,		// 1111, extra-tall pixels, typically 1024-1600 pixels per line
};

enum palette {
	PAL_CTIA = 0,			// 000, 128 colors YC, from the original Atari 8-bit

	PAL_SPECTRUM,			// 010, 15/16 colors, ZX Spectrum (two blacks)
	PAL_C64,				// 011, 16-color Commodore 64
	PAL_CPC,				// 100, 27-color RGB (Amstrad CPC)
	PAL_RGB3,				// 101 512-color RGB (Atari ST)
	PAL_RGB4,				// 110, 4096-color RGB (Amiga OCS, Atari STe)

	PAL_RGB1,				// 1110000, 8-color RGB (BBC Micro, QL)
	PAL_RGB2,				// 1110001, 64-color RGB (EGA)
	PAL_RGB5,				// 1110010, 32768-color RGB (uncommon)
	PAL_RGB6,				// 1110011, 262144-color RGB (VGA, Atari Falcon)
	PAL_RGB8,				// 1110100, 16777216 colors (Amiga AGA)
	PAL_SAM,				// 1110101, 128 colors, low bit shared between RGB (Sam Coupe)
	PAL_APPLEII_LOW,		// 11101100, 15/16 colors, Apple II (two grays)
	PAL_APPLEII_HIGH,		// 11101101, 6/8 colors, Apple II (two blacks, two whites)
	PAL_TMS9918,			// 1110111, 15+1 colors, TI 99/4A, MSX
	PAL_YMV9938,			// 1111000, 256 colors, 3:3:2 RGB, MSX2
	PAL_CGA_IBM,			// 11110010, 16 colors, low bit shared between RGB, dark yellow -> dark orange
	PAL_CGA_CLONE,			// 11110011, 16 colors, low bit shared between RGB
	PAL_VIC20,				// 1111010, 16 colors
	PAL_PLUS4,				// 1111011, 121 colors, YC
	PAL_QL4,				// 1111100, 4 colors
	PAL_MO5,				// 111110100, 16 colors
	PAL_TO770_1,			// 111110101, 16 colors
	PAL_TO770_2,			// 111110110, 16 colors
	PAL_TO770_3,			// 111110111, 16 colors
	PAL_MC6847_RES_BLACK,	// 111111000, 2 colors
	PAL_MC6847_RES_GREEN,	// 111111001, 2 colors
	PAL_MC6847_COL_RGB,		// 111111010, 4 colors
	PAL_MC6847_COL_CMO,		// 111111011, 4 colors
	PAL_RESERVED,			// 1111111
};

enum lookup {
	LOOKUP_FULL,				// 00, unrestricted palette
	LOOKUP_COMPUTED_EHB,		// 0100, extra-half-brite, Amiga OCS rev2
	LOOKUP_COMPUTED_1C2L,		// 0101, 1 chroma 2 luma, Atari 8-bit
	LOOKUP_COMPUTED_1C16L,		// 0110, 1 chroma 16 luma, Atari 8-bit rev2
	LOOKUP_COMPUTED_1L16C,		// 0111, 1 luma 16 chroma, Atari 8-bit rev2
	LOOKUP_PARTIAL_CGA_40LO,	// 10000, CGA 4-color
	LOOKUP_PARTIAL_CGA_40HI,	// 10001, CGA 4-color
	LOOKUP_PARTIAL_CGA_41LO,	// 10010, CGA 4-color
	LOOKUP_PARTIAL_CGA_41HI,	// 10011, CGA 4-color
	LOOKUP_PARTIAL_CGA_5LO,		// 10100, CGA 4-color (TV monochrome)
	LOOKUP_PARTIAL_CGA_5HI,		// 10101, CGA 4-color (TV monochrome)
	LOOKUP_PARTIAL_CGA_6,		// 1011, CGA 2-color
	LOOKUP_FIXED,				// 11, fixed colors, no palette
};

struct image {
	int width;
	int height;
	enum pixel_aspect_ratio par;
	int bpp;
	enum palette palette;
	enum lookup lookup;
	unsigned char separate_border;

	unsigned char color_used[256];

	unsigned char red[257];
	unsigned char green[257];
	unsigned char blue[257];
	unsigned char luma[257];
	unsigned char chroma[257];
};

#endif /* IMAGE_H_INCLUDED */
