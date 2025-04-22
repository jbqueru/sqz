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

#include "filetypes.h"

#include "debug.h"
#include "exitcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct {
	char* extension;
	enum filetypes type;
} filetype_mapping[] =
{
	{ "pi1", FILETYPE_PI1 },
	{ "qs1", FILETYPE_QS1 },
	{ NULL, FILETYPE_UNKNOWN }
};

enum filetypes filetype_from_filename(char const *const filename) {
	char const *extension = strrchr(filename, '.');
	if (!extension) {
		if (verbosity >= VERB_EXTRA) {
			printf("no extension\n");
		}
		return FILETYPE_UNKNOWN;
	}
	if (verbosity >= VERB_EXTRA) {
		printf("found extension %s, ", extension);
	}
	++extension;
	for (int i = 0; filetype_mapping[i].extension; ++i) {
		if (strcasecmp(extension, filetype_mapping[i].extension) == 0) {
			if (verbosity >= VERB_EXTRA) {
				printf("extension recognized\n");
			}
			return filetype_mapping[i].type;
		}
	}
	if (verbosity >= VERB_EXTRA) {
		printf("extension not recognized\n");
	}
	return FILETYPE_UNKNOWN;
}
