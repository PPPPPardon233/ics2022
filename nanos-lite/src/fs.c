#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t prooff;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *path, int flags, int mode)
{
	int k = 0;
	for (int i = 0; i < (sizeof(file_table) / sizeof(Finfo)); i++, k++)
	{
		if (strcmp(path, file_table[i].name) == 0)
		{
			Log("fs_open file name %s", file_table[i].name);
			file_table[i].prooff = 0;
			return i;
		}
	}
	assert(k != sizeof(file_table) / sizeof(Finfo));
	return -1;
}

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t fs_read(int fd, void *buf, size_t length)
{
	size_t ret = -1;
	size_t need_to_read = 0;
	size_t fsize = file_table[fd].size;
	size_t prooff = file_table[fd].prooff;

	if ( length > fsize - prooff ) {
		need_to_read = fsize - prooff;
	} 
  else {
		need_to_read = length;
	}

	if ( need_to_read <= 0 ) {
		need_to_read = 0;
	}


	if (file_table[fd].read == NULL)
	{
		ret = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].prooff, need_to_read);
		file_table[fd].prooff += need_to_read;
	}
	else
	{
		return file_table[fd].read(buf, 0, length);
	}

	return ret;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
	size_t ret = -1;
	size_t wlength = 0;
	size_t filesz = file_table[fd].size;
	size_t prooff = file_table[fd].prooff;

	if ( len > filesz - prooff ) {
		wlength = filesz - prooff;
	} else {
		wlength = len;
	}

	if ( wlength <= 0 ) wlength = 0;

	size_t offset = file_table[fd].disk_offset + file_table[fd].prooff;
	if (file_table[fd].write == NULL)
	{
		ret = ramdisk_write(buf, offset, wlength);
		file_table[fd].prooff += wlength;
	}
	else
	{
		return file_table[fd].write(buf, offset, len);
	}

	return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
	if (whence == SEEK_SET ) {
		file_table[fd].prooff = offset;
	} else if (whence == SEEK_CUR) {
		file_table[fd].prooff += offset;
	} else if ( whence == SEEK_END ) {
		file_table[fd].prooff = file_table[fd].size + offset;
	} else {
		panic("Unknown whence %d", whence);
	}
	
	return file_table[fd].prooff;
}

int fs_close(int fd)
{
	file_table[fd].prooff = 0;
	return 0;
}