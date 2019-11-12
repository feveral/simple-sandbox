#ifndef SANDBOX_LIB_H
#define SANDBOX_LIB_H

#include <dirent.h>
#include <sys/types.h>

void *getOriginFunction(const char *command);
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
DIR *opendir(const char *name);
int execv(const char *path, char *const argv[]);
int execve(const char *filename, char *const argv[], char *const envp[]);

#endif // SANDBOX_LIB_H