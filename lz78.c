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

#include "lz78_internal.h"

#include "debug.h"
#include "exitcodes.h"

#include <stdio.h>
#include <stdlib.h>

lz78encoder* lz78encoder_construct() {
	lz78encoder* that = calloc(1, sizeof(lz78encoder));
	if (!that) {
		fprintf(stderr, FL "Can't allocate lz78encoder structure (%zu bytes)\n", sizeof (lz78encoder));
		exit(EXIT_MEMORY);
	}
	return that;
}

void lz78encoder_destruct(lz78encoder *const that) {
	free(that);
}
