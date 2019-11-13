#include "sandboxLib.h"
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdarg.h>
#include <fcntl.h>

int debugoutput;
int stdoutfd;
__attribute__((constructor)) static void init(void) {
    int debug = 0;
    if (debug) debugoutput = getstdout(); 
    else debugoutput = getdevnull();
    stdoutfd = getstdout();
}

int isContain(const char *str, char target)
{
    for(int i = 0; i < strlen(str); i++)
        if (str[i] == target) return 1;
    return 0;
}

char *cutPathTail(const char *path)
{
    char *cutpath = malloc(sizeof(char) * (strlen(path)+1) );
    memset(cutpath, 0, sizeof(cutpath));
    strncpy(cutpath, path, strlen(path));
    for(int i=strlen(cutpath)-1; i>=0;i--){
        if (cutpath[i] == '/') {
            cutpath[i+1] = '\0';
            break;
        }
    }
    return cutpath;
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void *getOriginFunction(const char *command)
{
    void *handle = dlopen("libc.so.6", RTLD_LAZY);
    void *func = dlsym(handle, command);
    return func;
}

int getdevnull()
{
    int (*func)(const char *pathname, int flags, ...);
    func = getOriginFunction("open");
    int devnull = func("/dev/null", O_WRONLY);
    return devnull;
}

int getstdout()
{
    int (*func)(const char *pathname, int flags, ...);
    func = getOriginFunction("open");
    int stdoutfd = func("/dev/tty", O_WRONLY);
    return stdoutfd;
}

int checkPathCreate(const char *pathname, char *command)
{
    char *path;
    if (!isContain(pathname, '/')) path = "./";
    else path = cutPathTail(pathname);

    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *abs_parent = realpath(cwd, NULL);
        char *abs_child  = realpath(path, NULL);
        if (abs_parent == 0 || abs_child == 0) {
            dprintf(stdoutfd, "[sandbox] %s: access to %s is not allowed\n", command, pathname); 
            return 0;
        }
        size_t parent_len = strlen(abs_parent);
        int result = strncmp(abs_parent, abs_child, parent_len) == 0;
        if (result == 0) {
            dprintf(stdoutfd, "[sandbox] %s: access to %s is not allowed\n", command, pathname); 
            return 0;
        } else return 1;
    } else return 0;
}

int checkPath(const char *path, char *command)
{
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char abs_parent[512];
        char abs_child[512];
        realpath(cwd, abs_parent);
        realpath(path, abs_child);
        if (abs_parent == 0 || abs_child == 0) {
            dprintf(stdoutfd, "[sandbox] %s: access to %s is not allowed\n", command, path); 
            return 0;
        }
        size_t parent_len = strlen(abs_parent);
        int result = strncmp(abs_parent, abs_child, parent_len) == 0;
        if (result == 0) {
            dprintf(stdoutfd, "[sandbox] %s: access to %s is not allowed\n", command, path); 
            return 0;
        } else return 1;
    } else return 0;
}

int execl(const char *path, const char *arg, ...)
{
    printf("[sandbox] execl(%s): not allowed\n", path); return -1;
}

int execle(const char *path, const char *arg, ...)
{
    printf("[sandbox] execle(%s): not allowed\n", path); return -1;
}

int execlp(const char *file, const char *arg, ...)
{
    printf("[sandbox] execlp(%s): not allowed\n", file); return -1;
}

int execv(const char *path, char *const argv[])
{
    printf("[sandbox] execv(%s): not allowed\n", path); return -1;
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
    printf("[sandbox] execve(%s): not allowed\n", filename); return -1;
}

int execvp(const char *file, char *const argv[])
{
    printf("[sandbox] execvp(%s): not allowed\n", file); return -1;
}

int system(const char *command)
{
    printf("[sandbox] system(%s): not allowed\n", command); return -1;
}

int chdir(const char *path)
{
    dprintf(debugoutput, "chdir is called: %s\n", path);
    if (checkPath(path, "chdir") == 1) {
        int (*func)(const char *path);
        func = getOriginFunction("chdir");
        return func(path);
    } return -1;
}

int chmod(const char *pathname, mode_t mode)
{
    dprintf(debugoutput, "chmod is called: %s\n", pathname);
    if (checkPath(pathname, "chmod") == 1) {
        int (*func)(const char *pathname, mode_t mode);
        func = getOriginFunction("chmod");
        return func(pathname, mode);
    } return -1;
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags)
{
    dprintf(debugoutput, "fchmodat is called: %s\n", pathname);
    if (checkPath(pathname, "fchmodat")) {
        int (*func)(int dirfd, const char *pathname, mode_t mode, int flags);
        func = getOriginFunction("fchmodat");
        return func(dirfd, pathname, mode, flags);
    } return -1;
}

int chown(const char *pathname, uid_t owner, gid_t group)
{
    dprintf(debugoutput, "chown is called: %s\n", pathname);
    if (checkPath(pathname, "chown") == 1) {
        int (*func)(const char *pathname, uid_t owner, gid_t group);
        func = getOriginFunction("chown");
        return func(pathname, owner, group);
    } return -1;
}

int creat(const char *pathname, mode_t mode)
{
    dprintf(debugoutput, "creat is called: %s\n", pathname);
    if (checkPath(pathname, "creat") == 1) {
        int (*func)(const char *pathname, mode_t mode);
        func = getOriginFunction("creat");
        return func(pathname , mode);
    } return -1;
}

FILE *fopen(const char *path, const char *mode)
{
    dprintf(debugoutput, "fopen is called: %s\n", path);
    if (checkPath(path, "fopen") == 1) {
        FILE* (*func)(const char *path, const char *mode);
        func = getOriginFunction("fopen");
        return func(path, mode);
    } return 0;
}

int link(const char *oldpath, const char *newpath)
{
    dprintf(debugoutput, "link is called: %s, %s\n", oldpath, newpath);    
    if (checkPath(oldpath, "link") && checkPath(newpath, "link")) {
        int (*func)(const char *oldpath, const char *newpath);
        func = getOriginFunction("link");
        return func(oldpath, newpath);
    } return -1;
}

int mkdir(const char *pathname, mode_t mode)
{
    dprintf(debugoutput, "mkdir is called: %s\n", pathname);
    if (checkPath(pathname, "mkdir") == 1) {
        int (*func)(const char *pathname, mode_t mode);
        func = getOriginFunction("mkdir");
        return func(pathname, mode);
    } return 0;
}

DIR *opendir(const char *name)
{
    dprintf(debugoutput, "opendir is called: %s\n", name);
    if (checkPath(name, "opendir") == 1) {
        DIR* (*func)(const char *name);
        func = getOriginFunction("opendir");
        return func(name);
    } return 0;
}

int open(const char *pathname, int flags, ...)
{
    dprintf(debugoutput, "open is called: %s\n", pathname);
    if (checkPath(pathname, "open") == 1) {
        int (*func)(const char *pathname, int flags, ...);
        func = getOriginFunction("open");
        va_list ap;
        mode_t arg;
        va_start(ap, flags);
        arg = va_arg(ap, mode_t);
        va_end(ap);
        if (arg != 0) {
            return func(pathname, flags, arg);
        } else {
            return func(pathname, flags);
        }
    } return -1;
}

int openat(int dirfd, const char *pathname, int flags, ...)
{
    dprintf(debugoutput, "openat is called: %s\n", pathname);
    if (checkPath(pathname, "openat") == 1) {
        int (*func)(int dirfd, const char *pathname, int flags, ...);
        func = getOriginFunction("openat");
        va_list ap;
        mode_t arg;
        va_start(ap, flags);
        arg = va_arg(ap, mode_t);
        va_end(ap);
        if (arg != 0) {
            return func(dirfd, pathname, flags, arg);
        } else {
            return func(dirfd, pathname, flags);
        }
    } return -1;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
    dprintf(debugoutput, "readlink is called: %s\n", pathname);
    if (checkPath(pathname, "readlink") == 1) {
        int (*func)(const char *pathname, char *buf, size_t bufsiz);
        func = getOriginFunction("readlink");
        return func(pathname, buf, bufsiz);
    } return -1;
}

int remove(const char *pathname)
{
    dprintf(debugoutput, "remove is called: %s\n", pathname);
    if (checkPath(pathname, "remove") == 1) {
        int (*func)(const char *pathname);
        func = getOriginFunction("remove");
        return func(pathname);
    } return -1;
}

int rename(const char *oldpath, const char *newpath)
{
    dprintf(debugoutput, "rename is called: %s, %s\n", oldpath, newpath);
    if (checkPath(oldpath, "rename") && checkPath(newpath, "rename")) {
        int (*func)(const char *oldpath, const char *newpath);
        func = getOriginFunction("rename");
        return func(oldpath, newpath);
    } return -1;
}

int rmdir(const char *pathname)
{
    dprintf(debugoutput, "rmdir is called: %s\n", pathname);
    if (checkPath(pathname, "rmdir")) {
        int (*func)(const char *pathname);
        func = getOriginFunction("rmdir");
        return func(pathname);
    } return -1;
}

int __xstat(int ver, const char *path, struct stat *buf)
{
    dprintf(debugoutput, "__xstat is called: %s\n", path);
    if (checkPath(path, "__xstat")) {
        int (*func)(int ver, const char *path, struct stat *buf);
        func = getOriginFunction("__xstat");
        return func(ver, path, buf);
    } return -1;
}

// int __fxstat(int ver, int fildes, struct stat *buf)
// {
//     printf("__fxstat is called\n");
//     if (1) {
//         int (*func)(int ver, const char *path, struct stat *buf);
//         func = getOriginFunction("__fxstat");
//         return func(ver, fildes, buf);
//     } return -1;
// }

int __lxstat(int ver, const char *path, struct stat *buf)
{
    dprintf(debugoutput, "__lxstat is called: %s\n", path);
    if (checkPath(path, "__lxstat")) {
        int (*func)(int ver, const char *path, struct stat *buf);
        func = getOriginFunction("__lxstat");
        return func(ver, path, buf);
    } return -1;
}

int symlink(const char *target, const char *linkpath)
{
    dprintf(debugoutput, "symlink is called: %s, %s\n", target, linkpath);
    if (checkPath(target, "symlink") && checkPathCreate(linkpath, "symlink")) {
        int (*func)(const char *target, const char *linkpath);
        func = getOriginFunction("symlink");
        return func(target, linkpath);
    } return -1;
}

// Haven't test
int unlink(const char *pathname)
{
    dprintf(debugoutput, "unlink is called: %s\n", pathname);
    if (checkPath(pathname, "unlink")) {
        int (*func)(const char *pathname);
        func = getOriginFunction("unlink");
        return func(pathname);
    } return -1;
}

// Haven't test
int unlinkat(int dirfd, const char *pathname, int flags)
{
    dprintf(debugoutput, "unlinkat is called: %s\n", pathname);
    if (checkPath(pathname, "unlinkat")) {
        int (*func)(int dirfd, const char *pathname, int flags);
        func = getOriginFunction("unlinkat");
        return func(dirfd, pathname, flags);
    } return -1;
}