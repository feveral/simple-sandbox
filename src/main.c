#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <dlfcn.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "utility.h"

int main(int argc, char *argv[])
{
    char *basePath = "./";
    char *sandboxLibPath = "./sandbox.so";
    int opt, flag = 0;
    while ((opt = getopt(argc, argv, ":dp")) != -1) {
        if (opt == 'd') {
            basePath = argv[optind];
            flag = 1;
        } else if (opt == 'p') {
            sandboxLibPath = argv[optind];
            flag = 1;
        } else {
            printf("./sandbox: invalid option -- '%c'\n", argv[optind-1][1]);
            printString("usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]");
            printString("-p: set the path to sandbox.so, default = ./sandbox.so");
            printString("-d: restrict directory, default = .");
            printString("--: seperate the arguments for sandbox and for the executed command");
            return 0;
        }
    }

    char *command = "";
    for(int j = optind+flag; j < argc; j++) {
        char *oldCommand = command;
        oldCommand = concat(oldCommand, argv[j]);
        command = concat(oldCommand, " ");
    }

    char *ldPreload = concat("LD_PRELOAD=", realpath(sandboxLibPath,  NULL));
    ldPreload = concat(ldPreload, " ");
    chdir(basePath);
    system(concat(ldPreload, command));
    return 0;
}