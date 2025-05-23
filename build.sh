#!/bin/sh

# Copyright 2025 Jean-Baptiste M. "JBQ" "Djaybee" Queru
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

# SPDX-License-Identifier: AGPL-3.0-or-later

echo '(*) create output directories'
mkdir -p out/bin || exit $?
mkdir -p out/gfx || exit $?
mkdir -p out/tos || exit $?
mkdir -p out/tmp || exit $?

echo '(*) build bitstream tests'
gcc tests/test_bitstream.c bitstream.c exitcodes.c -O2 -Wall -Wextra -o out/bin/test_bitstream || exit $?

echo '(*) run bitstream tests'
out/bin/test_bitstream || exit $?

echo '(*) build Squeezer tool'
gcc \
sqz.c \
\
cmdline.c \
debug.c \
exitcodes.c \
filetypes.c \
license.c \
\
bitstream.c \
image.c \
\
other_formats/degas.c \
\
sqz_formats/qs.c \
\
huffman.c \
lz78.c \
\
-O2 -Wall -Wextra -o out/bin/sqz || exit $?

echo '(*) BUILD SUCCESSFUL'
