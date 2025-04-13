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

#ifndef EXITCODES_H_INCLUDED
#define EXITCODES_H_INCLUDED

extern const int EXIT_SUCCESS;
extern const int EXIT_MEMORY;
extern const int EXIT_CMDLINE;
extern const int EXIT_INPUTFILE;
extern const int EXIT_OUTPUTFILE;
extern const int EXIT_BADFILE;
extern const int EXIT_INVALIDSTATE;
extern const int EXIT_IMPLEMENTATION;

void display_help_exitcodes();

#endif /* EXITCODES_H_INCLUDED */
