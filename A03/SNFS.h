#ifndef _SNFS_H
#define _SNFS_H

//Struct for readdir requests

char* remote_getattr(const char *path);
char* remote_readdir(const char *path);
char* remote_mkdir(const char *path, mode_t mode);
char* remote_releasedir(const char *pointer);
char* remote_opendir(const char *path);





#endif
