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

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#define SS(x) SSS(x)
#define SSS(x) #x
#define FL __FILE__ ":" SS(__LINE__) ": "

extern int verbosity;

extern const int VERB_QUIET;
extern const int VERB_NORMAL;
extern const int VERB_VERBOSE;
extern const int VERB_EXTRA;

#endif /* DEBUG_H_INCLUDED */
