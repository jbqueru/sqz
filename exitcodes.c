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

#include "exitcodes.h"

#include <stdio.h>

const int EXIT_SUCCESS = 0;
const int EXIT_MEMORY = 1;
const int EXIT_CMDLINE = 2;
const int EXIT_INPUTFILE = 3;
const int EXIT_OUTPUTFILE = 4;
const int EXIT_INVALIDSTATE = 5;

void display_help_exitcodes() {
    printf("Exit codes:\n");
    printf("0: success\n");
    printf("1: memory allocaton failure\n");
    printf("2: invalid command line\n");
    printf("3: I/O error on input file\n");
    printf("4: I/O error on output file\n");
    printf("5: invalid internal state\n");
}
