/*
 * rfs.cpp
 *
 *  Created on: 28 мая 2021 г.
 *      Author: alex9932
 */

#include "rfs.h"
#include "utils.h"
#include <rg_filesystem.h>

#ifdef _WIN32
#error "Win32 is not supported!"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#define RFS_DIR  4
#define RFS_FILE 8

static rg_string _in  = NULL;
static rg_string _out = NULL;

static std::vector<char*> file_list;
static long int offset = 0;
static long int foffset = 0;

static int _strcends(char* string, char c) {
	int length = strlen(string);
	for (int var = length-1; var >= 0; --var) {
		if(string[var] == c) { return var; }
	}
	return -1;
}

static void _processDir(rg_string path) {
#ifdef _WIN32
#error "Win32 is not supported!"
#else
	char _path[256];
	memset(_path, 0, 256);
	strcat(_path, path);
	strcat(_path, "/");
	DIR* dir = opendir(path);
	struct dirent* ent;
	while((ent = readdir(dir)) != 0) {
		if(!rg_streql(ent->d_name, ".") && !rg_streql(ent->d_name, "..")) {
			char __path[256];
			memset(__path, 0, 256);
			strcat(__path, _path);
			strcat(__path, ent->d_name);
			if(ent->d_type == RFS_DIR) {
				_processDir(__path);
			} else if(ent->d_type == RFS_FILE) {
				char* str = (char*)malloc(256);
				memset(str, 0, 256);
				if(__path[0] == '.' && __path[1] == '/') {
					strcat(str, &__path[2]);
				} else if(__path[0] == '/') {
					strcat(str, &__path[1]);
				} else {
					strcat(str, __path);
				}
				file_list.push_back(str);
			}
		}
	}
	closedir(dir);
#endif
}

void rfs_pack(rg_string in, rg_string out) {
	file_list.clear();
	_in = in;
	_out = out;
	_processDir(in);

	rg_fs_header header;
	header.magic[0] = 'r'; header.magic[1] = 'f'; header.magic[2] = 's'; header.magic[3] = '1';
	header.files = file_list.size();
	offset += sizeof(rg_fs_header);
	offset += sizeof(rg_fs_file) * file_list.size();

	FILE* f_out = fopen(out, "wb");
	fwrite(&header, sizeof(rg_fs_header), 1, f_out);
	printf("Writing...\n");

	for (unsigned int var = 0; var < file_list.size(); ++var) {
		rg_fs_file file;
		char* f_path = file_list[var];
		FILE* f = fopen(f_path, "rb");
		fseek(f, 0, SEEK_END);
		long int length = ftell(f);
		file.length = length;
		memset(file.name, 0, 256);
		strcat(file.name, f_path);
		file.offset = offset + foffset;
		foffset += length;
		fwrite(&file, sizeof(rg_fs_file), 1, f_out);
	}

	for (unsigned int var = 0; var < file_list.size(); ++var) {
		char* f_path = file_list[var];
		printf("%s", f_path);
		for (unsigned int var = 0; var < 128 - strlen(f_path); ++var) { printf(" "); }
		FILE* f = fopen(f_path, "rb");
		if(f) {
			fseek(f, 0, SEEK_END);
			long int length = ftell(f);
			fseek(f, 0, SEEK_SET);
			void* data = malloc(length);
			fread(data, 1, length, f);
			fclose(f);
			fwrite(data, 1, length, f_out);
			free(file_list[var]);
			printf("ok!\n");
		} else {
			printf("skipped!\n");
			printf("Packing can't be continued!\n");
			return;
		}
	}
	fclose(f_out);
}

void rfs_unpack(rg_string in, rg_string out) {
#ifdef _WIN32
#error "Win32 is not supported!"
#else
	struct stat st = {0};
#endif

	FILE* f = fopen(in, "rb");
	if(f) {
		printf("Reading...\n");
		rg_filesystem* fs = (rg_filesystem*)malloc(sizeof(rg_filesystem));
		fs->file_stream = f;
		fread(&fs->header, sizeof(rg_fs_header), 1, fs->file_stream);
		fs->files = (rg_fs_file*)malloc(sizeof(rg_fs_file) * fs->header.files);
		fread(fs->files, sizeof(rg_fs_file), fs->header.files, fs->file_stream);
		for (unsigned int var = 0; var < fs->header.files; ++var) {
			rg_fs_file file = fs->files[var];
			printf("%s", file.name);
			fseek(f, file.offset, SEEK_SET);
			void* data = malloc(file.length);
			fread(data, 1, file.length, f);
			int sep_pos = _strcends(file.name, '/');
			if(sep_pos != -1) {
				char* dir = (char*)malloc(sep_pos + 1);
				memset(dir, 0, sep_pos + 1);
				memcpy(dir, file.name, sep_pos);
#ifdef _WIN32
#eror "Win32 is not supported!"
#else
				if (!rg_streql(dir, ".") && stat(dir, &st) == -1) {
				    mkdir(dir, 0700);
				}
#endif
				free(dir);
			}

			FILE* f_out = fopen(file.name, "wb");
			for (unsigned int var = 0; var < 128 - strlen(file.name); ++var) { printf(" "); }
			if(f_out) {
				fwrite(data, 1, file.length, f_out);
				fclose(f_out);
				printf("ok!\n");
			} else {
				printf("skipped!\n");
			}
			free(data);
		}
		free(fs->files);
		free(fs);
	}
	fclose(f);
}
