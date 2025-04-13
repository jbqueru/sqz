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

// The header that matches this souce file
#include "ucmdline.h"

// Other private headers we use, in alphabetical order
#include "debug.h"
#include "exitcodes.h"
#include "license.h"

// Public headers we use, in alphabetical order
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* cmdline_inputfilename;
char* cmdline_outputfilename;
compressed_format cmdline_compressed_format;

void parse_cmdline(int argc, char** argv) {
	cmdline_inputfilename = NULL;
	cmdline_outputfilename = NULL;
	verbosity = VERB_NORMAL;
	if (argc == 1) {
		display_version();
		printf("\n");
		display_help(argv[0]);
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
			display_help(argv[0]);
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
		if (!strcmp(argv[i], "--format")) {
			i++;
			if (i >= argc) {
				fprintf(stderr, "--format specified without format\n");
				exit(EXIT_CMDLINE);
			}
			if (cmdline_compressed_format) {
				fprintf(stderr, "Multiple input formats found: %s\n", argv[i]);
				exit(EXIT_CMDLINE);
			}
			if (!strcmp(argv[i], "implicit")) {
				cmdline_compressed_format = FORMAT_IMPLICIT;
			}
			if (!strcmp(argv[i], "qs1")) {
				cmdline_compressed_format = FORMAT_QS1;
			}
			if (!cmdline_compressed_format) {
				fprintf(stderr, "Unrecognized input format: %s\n", argv[i]);
				exit(EXIT_CMDLINE);
			}
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
			}
			cmdline_outputfilename = strdup(argv[i]);
			if (!cmdline_outputfilename) {
				fprintf(stderr, FL"Could not allocate output filename\n");
				exit(EXIT_MEMORY);
			}
			continue;
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
	if (verbosity >= VERB_VERBOSE) {
		printf("input filename : %s\n", cmdline_inputfilename);
		printf("output filename : %s\n", cmdline_outputfilename);
	}
	if (cmdline_compressed_format <= FORMAT_IMPLICIT) {
		size_t filename_length = strlen(cmdline_inputfilename);
		if (filename_length <= 4) {
			fprintf(stderr, "no format specified, filename too short to guess: %s\n", cmdline_inputfilename);
			exit(EXIT_CMDLINE);
		}
		if (!strcasecmp(cmdline_inputfilename + filename_length - 4, ".qs1")) {
			cmdline_compressed_format = FORMAT_QS1;
		}
		if (cmdline_compressed_format <= FORMAT_IMPLICIT) {
			fprintf(stderr, "no format specified, cound not guess from filename: %s\n", cmdline_inputfilename);
			exit(EXIT_CMDLINE);
		}
	}
	if (verbosity >= VERB_VERBOSE) {
		switch(cmdline_compressed_format) {
			case FORMAT_QS1: printf("format: QS1 (Atari ST low-resolution)\n"); break;
			default: break;
		}
	}
}

void display_version() {
	printf("Unsqueezer version 0.0 (devel)\n");
	printf("\n");
	printf("A decompression program for retrocomputing use cases\n");
	printf("\n");
	display_license();
}

void display_help(char const *const program_name) {
	printf("Usage: %s <inputfile> [option...]\n", program_name);
	printf("\n");
	printf("Command-line options:\n");
	printf("--help: print this help message to stdout\n");
	printf("--version: print version information to stdout\n");
	printf("\n");
	printf("--quiet: suppress all output\n");
	printf("--verbose: additional output\n");
	printf("--extraverbose: even more additional output\n");
	printf("\n");
	printf("--format\n");
	printf("--output\n");
	display_help_exitcodes();
}
