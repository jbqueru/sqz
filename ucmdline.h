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

#ifndef UCMDLINE_H_INCLUDED
#define UCMDLINE_H_INCLUDED

typedef enum {
    FORMAT_UNDEFINED = 0,
    FORMAT_IMPLICIT,
    FORMAT_QS1,
} compressed_format;

extern char* cmdline_inputfilename;
extern char* cmdline_outputfilename;
extern compressed_format cmdline_compressed_format;

void parse_cmdline(int argc, char** argv);
void display_version();
void display_help(char const *const program_name);

#endif /* UCMDLINE_H_INCLUDED */
