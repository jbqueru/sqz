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

#include "cmdline.h"

#include "debug.h"
#include "exitcodes.h"
#include "license.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* cmdline_inputfilename;
char* cmdline_outputfilename;

void parse_cmdline(int argc, char** argv) {
	cmdline_inputfilename = NULL;
	cmdline_outputfilename = NULL;
	verbosity = VERB_NORMAL;
	if (argc == 1) {
		display_version();
		printf("\n");
		display_help();
		exit(EXIT_CMDLINE);
	}
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--version")) {
			if (i > 1 || argc > 2) {
				fprintf(stderr, "--version can't be used with other options\n");
				exit(EXIT_CMDLINE);
			}
			display_version();
			exit(EXIT_SUCCESS);
		}
		if (!strcmp(argv[i], "--help")) {
			if (i > 1 || argc > 2) {
				fprintf(stderr, "--help can't be used with other options\n");
				exit(EXIT_CMDLINE);
			}
			display_help();
			exit(EXIT_SUCCESS);
		}

		if (!strcmp(argv[i], "--quiet")) {
			verbosity = VERB_QUIET;
			continue;
		}
		if (!strcmp(argv[i], "--verbose")) {
			verbosity = VERB_VERBOSE;
			continue;
		}
		if (!strcmp(argv[i], "--extraverbose")) {
			verbosity = VERB_EXTRA;
			continue;
		}
		if (!strcmp(argv[i], "--output")) {
			i++;
			if (i >= argc) {
				fprintf(stderr, "--output specified without filename\n");
				exit(EXIT_CMDLINE);
			}
			if (cmdline_outputfilename) {
				fprintf(stderr, "Multiple output filenames found: %s\n", argv[i]);
				exit(EXIT_CMDLINE);
			} else {
				cmdline_outputfilename = strdup(argv[i]);
				if (!cmdline_outputfilename) {
					fprintf(stderr, FL"Could not allocate output filename\n");
					exit(EXIT_MEMORY);
				}
				continue;
			}
		}

		if (argv[i][0] == '-') {
			fprintf(stderr, "Unrecognized option %s\n", argv[i]);
			exit(EXIT_CMDLINE);
		}
		if (cmdline_inputfilename) {
			fprintf(stderr, "Multiple input filenames found: %s\n", argv[i]);
			exit(EXIT_CMDLINE);
		} else {
			cmdline_inputfilename = strdup(argv[i]);
			if (!cmdline_inputfilename) {
				fprintf(stderr, FL"Could not allocate input filename\n");
				exit(EXIT_MEMORY);
			}
		}
	}
	if (!cmdline_inputfilename) {
		fprintf(stderr, "No input filename spcified\n");
		exit(EXIT_CMDLINE);
	}
	if (verbosity >= VERB_NORMAL) {
		display_license();
		printf("\n");
	}
	if (verbosity >= VERB_EXTRA) {
		printf("input filename : %s\n", cmdline_inputfilename);
		printf("output filename : %s\n", cmdline_outputfilename);
	}
}

void display_version() {
	printf("Squeezer version 0.0 (devel)\n");
	printf("\n");
	printf("A compression program for retrocomputing use cases\n");
	printf("\n");
	display_license();
}

void display_help() {
	printf("Command-line options:\n");
	printf("--help: print this help message to stdout\n");
	printf("--version: print version information to stdout\n");
	printf("\n");
	printf("--quiet: suppress all output\n");
	printf("--verbose: additional output\n");
	printf("--extraverbose: even more additional output\n");
	printf("\n");
	display_help_exitcodes();
}
