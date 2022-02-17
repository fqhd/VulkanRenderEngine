#include <stdlib.h>
#include <stdio.h>

typedef struct {
	void* data;
	int size;
} file_buffer;

file_buffer read_file(const char* file_path);
void free_file(file_buffer* buffer);