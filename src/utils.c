#include "utils.h"

file_buffer read_file(const char* file_path){
	FILE* file = fopen(file_path, "rb");
	int file_size = fseek(file, 0, SEEK_END);
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