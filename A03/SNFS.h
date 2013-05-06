#ifndef _SNFS_H
#define _SNFS_H

//Struct for readdir requests

char* remote_getattr(const char *path);
char* remote_readdir(const char *path);
char* remote_mkdir(const char *path, mode_t mode);
char* remote_releasedir(const char *pointer);
char* remote_opendir(const char *path);
char* remote_read(const char *pointer, size_t size, off_t offset);
char* remote_open(const char *path, int flags);
char* remote_release(const char *pointer);
char* remote_write(const char *pointer, size_t size, off_t offset, char* buffer);
char* remote_truncate(const char *path, off_t newsize);
char* remote_create(const char *path, mode_t mode);

#endif
