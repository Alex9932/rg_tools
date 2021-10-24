/*
 * main.cpp
 *
 *  Created on: 25 мая 2021 г.
 *      Author: alex9932
 */

#define RG_TOOL_VERSION "1.0"

#include <chrono>
#include <ctime>

#include <stdio.h>
#include <rg_engine.h>

#include "utils.h"
#include "rfs.h"
#include "rml.h"

#define RG_TOOL_NONE 0x00
#define RG_TOOL_RFS  0x01
#define RG_TOOL_RML  0x02

#define RG_MODE_NONE   0x00
#define RG_MODE_PACK   0x01
#define RG_MODE_UNPACK 0x02

static char tool = RG_TOOL_NONE;
static char mode = RG_MODE_NONE;
static rg_string input  = NULL;
static rg_string output = NULL;

static double _time = 0;

void printHelp() {
	printf("~ ~ ~ HELP MENU ~ ~ ~\n\n");
	printf(" Options:\n");
	printf("  -i   ( --input     ) -> Set input file / dir.\n");
	printf("  -o   ( --output    ) -> Set output file / dir.\n");
	printf("  -p   ( --pack      ) -> Set pack(/fml1) mode.\n");
	printf("  -u   ( --unpack    ) -> Set unpack(/fml2) mode.\n\n");
	printf(" Tools:\n");
	printf("  -fs  (     ---     ) -> Use fs tool.\n");
	printf("  -rml (     ---     ) -> Use rml tool.\n\n");
	printf(" Examples:\n");
	printf("  rg_tools -fs -p -i ./resdir -o ./respack.rgm\n");
	printf("  rg_tools -rml -p -i ./model.fml1 -o ./model.rgm (From fml v1 to rml)\n");
	printf("  rg_tools -rml -u -i ./model.fml2 -o ./model.rgm (From fml v2 to rml)\n\n");
	printf("~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
}

void printSum() {
	printf("Done at: %lfs.\n", _time);
	printf("Input: %s\n", input);
	printf("Output: %s\n", output);
}

int parseArgs(int argc, rg_string argv[]) {
	printf("Parsing arguments...\n");

	for (int i = 0; i < argc; i++) {
		rg_string arg = argv[i];

		// Set tool
		if (rg_streql(arg, "-fs")) {
			tool = RG_TOOL_RFS;
		}
		if (rg_streql(arg, "-rml")) {
			tool = RG_TOOL_RML;
		}

		// Set mode
		if (rg_streql(arg, "-p") || rg_streql(arg, "--pack")) {
			mode = RG_MODE_PACK;
		}
		if (rg_streql(arg, "-u") || rg_streql(arg, "--unpack")) {
			mode = RG_MODE_UNPACK;
		}

		// Set input/output files/dirs
		if (rg_streql(arg, "-i") || rg_streql(arg, "--input")) {
			if (i + 1 >= argc) {
				return -1;
			}
			input = argv[i + 1];
		}
		if (rg_streql(arg, "-o") || rg_streql(arg, "--output")) {
			if (i + 1 >= argc) {
				return -1;
			}
			output = argv[i + 1];
		}
	}

	return 0;
}

int main(int argc, rg_string argv[]) {
	printf("rg_tool v%s\n", RG_TOOL_VERSION);
	auto start = std::chrono::system_clock::now();

	if(parseArgs(argc, argv) != 0 || tool == RG_TOOL_NONE || mode == RG_MODE_NONE || input == NULL || output == NULL) {
		printf("Invalid arguments!\n");
		printHelp();
	} else {
		switch (tool) {
			case RG_TOOL_RFS:
				if (mode == RG_MODE_PACK) {
					rfs_pack(input, output);
				} else {
					rfs_unpack(input, output);
				}
				break;
			case RG_TOOL_RML:
				if (mode == RG_MODE_PACK) {
					rml_fml1(input, output);
				} else {
					rml_fml2(input, output);
				}
				break;
			default:
				break;
		}
	}

	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
	_time = (double)elapsed_seconds.count();
	printSum();

	return 0;
}
