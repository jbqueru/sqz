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

char* fn() {
	long* symbol_counts = calloc(1 , sizeof(long));
	for (int i = 0; i < 1; i++) {
		symbol_counts[i] = 0;
	}
	symbol_counts[0]++;

	// count symbols with a nonzero count
	long number_of_nonzero_symbols = 0;
	if (symbol_counts[0] > 0) {
		number_of_nonzero_symbols++;
	}
	printf("non-zero symbols: %ld\n", number_of_nonzero_symbols);

/* This is where gcc issues a warning, where it seems to believe
 * that number_of_nonzero_symbols is zero. The number is actually
 * deterministic, it's 1.
 */
	return malloc(number_of_nonzero_symbols - 1);
}

int main() { return !fn(); }
