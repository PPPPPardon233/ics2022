#ifndef __FS_H__
#define __FS_H__

#include <common.h>

enum {SEEK_SET, SEEK_CUR, SEEK_END};


int fs_open(const char *pathname, int flags, int mode);
int fs_close(int f);
size_t fs_read(int f, void *buffer, size_t length);
size_t fs_write(int f, const void *buffer, size_t length);
size_t fs_lseek(int f, size_t offset, int whence);

#endif
