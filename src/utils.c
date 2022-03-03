#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

file_buffer read_file(const char* file_path){
	FILE* file = fopen(file_path, "rb");
	if(file == NULL){
		err("Failed to open file");
	}
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	void* data = malloc(file_size);
	fread(data, sizeof(unsigned char), file_size, file);
	fclose(file);
	file_buffer f = { data, file_size };
	return f;
}

void free_file(file_buffer* buffer){
	free(buffer->data);
	buffer->data = NULL;
	buffer->size = 0;
}

void err(const char* msg){
	printf("-------------------- ERROR ---------------------\n");
	printf("%s\n", msg);
	printf("------------------------------------------------\n");
}