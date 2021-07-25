#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "demon.h"

ssize_t daemonize(void){
    ssize_t fd;

    switch(fork()){  // become background process
        case -1: return -1;
        case 0: break;
        default: exit(EXIT_SUCCESS);
    }

    // Become leader of a new session
    if (setsid() == -1){ return -1;};
    
    // fork AGAIN, to make sure the program can never reacquire a controlling terminal, according 
    // to SYSV conventions
    switch(fork()){
        case -1: return -1;
        case 0 : break;
        default: exit(EXIT_SUCCESS);
    }
    
    // reset umask
    umask(0);
    
    //cd to /, in order not to prevent shutdown by oreventing the fs from being unmounted
    chdir("/");

    // close standard input, output, and error streams, and use /dev/null
    // instead, so that io operations don't fail
    close(STDIN_FILENO);
    fd = open("/dev/null", O_RDWR);
    // fd should be 0, because linux reuses the smallest fd number, and 
    // stdin (0) has just been closed, so it should've been reused now
    if (fd != STDIN_FILENO){return -1;};
    // dup2 duplicates the fd in $1 and uses the fd in $2, with the result
    // that both point to the same thing
    if (dup2(STDIN_FILENO,  STDOUT_FILENO) != STDOUT_FILENO){ return -1;};
    if (dup2(STDIN_FILENO,  STDERR_FILENO) != STDERR_FILENO){ return -1;};

    return 0;
}


