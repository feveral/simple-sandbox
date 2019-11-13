#ifndef SANDBOX_LIB_H
#define SANDBOX_LIB_H

#include <dirent.h>
#include <sys/types.h>

int isContain(const char *str, char target);
char *cutPathTail(const char *path);
char *concat(const char *s1, const char *s2);
void *getOriginFunction(const char *command);
int getdevnull();
int getstdout();
int checkPathCreate(const char *pathname, char *command);
int checkPath(const char *path, char *command);

#endif // SANDBOX_LIB_H