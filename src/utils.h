typedef struct {
	void* data;
	int size;
} file_buffer;

file_buffer read_file(const char* file_path);
void free_file(file_buffer* buffer);
void err(const char* msg);