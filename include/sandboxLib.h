#ifndef SANDBOX_LIB_H
#define SANDBOX_LIB_H

#include <dirent.h>
#include <sys/types.h>

int getdevnull();
int getstdout();
void *getOriginFunction(const char *command);
char *concat(const char *s1, const char *s2);
char *cutPathTail(const char *path);
int isContain(const char *str, char target);
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
DIR *opendir(const char *name);
int execv(const char *path, char *const argv[]);
int execve(const char *filename, char *const argv[], char *const envp[]);

#endif // SANDBOX_LIB_H