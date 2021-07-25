#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "udslog.h"
#include "clogger.h"
#include "demon.h"


/*
-----> abbreviations <--------------
    - sfd : socked file descriptor
    - AF : address family
    - un : unix
    - uds : unix domain socket
*/


/* Simple server listening on a unix domain socket; 
   clients can send it datagram messages, and it writes 
   everything it gets to a log file; 

   Thus this can be a very simple system logger implementation; 
   you could build on this by nothing the time and date etc of the messatge
   received etc
*/
// the process will daemonize by default; run the program with -f to force it to run in
// the foreground instead
size_t DAEMONIZE=1;

void print_usage(char *progname){
    puts("USAGE:"); 
    printf("\t -s <socket path> -l <logfile path>\n");
    printf("\t -f \t run in the foreground; don't daemonize\n"); 
    printf("\t -h \t print this message and exit\n\n");
    puts(" ");
    printf("The defaults for the logfile and socket are %s and %s, respectively.\n\n", LOG_PATH, SOCK_PATH);
}





int main(int argc, char *argv[]){
 
    char buffer[BUFF_SIZE];
    // zero the buffer
    memset(&buffer, 0, sizeof(buffer));

    // for optarg
    int opt;

    // path to socket; default that can be overriden through the -s option
    char *mysock = SOCK_PATH;
    // path to log file; default that can be overriden through the -l option
    char *logfile = LOG_PATH;
        
    // parse arguments
    while((opt = getopt(argc, argv, "!s:l:hf")) != -1){
        switch(opt){
            case 's':
                mysock= optarg;
                printf("mysock is %s\n", mysock);
                break;
            case 'l':
                logfile = optarg;
                printf("logfile is %s\n", logfile);
                break;
            case 'f':
                DAEMONIZE=0;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            //case '?':
                // unrecognized options are puts optopt
                //printf("unrecognized option : %i", optopt);
                //break;
        }
    }

    // daemonize if DAEMONIZE is set to 1
    if (DAEMONIZE){
        if (daemonize() == -1){
            perror("failed to daemonize. FATAL.");
            exit(EXIT_FAILURE);
        };
    };

/* // not needed anymore; they both have defaults, so the args are optional
    //if (!mysock || !logfile){  // let logfile be NULL, in which case Logger will use
    //stdout
    if (!mysock){
        puts("missing required argument(s)\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
*/
    /* initialize logger */
    struct Logger logger;
    Logger_init(&logger, logfile, true);
    // allow all levels
    Logger_set_level(&logger, LOG_INFO);
    
    // try and open the file for appending, creating it if it doesn't exist
    // socket file descriptor; returned by socket()
    int  sfd=0;
    // the unix domain socket struct; passed around by various things
    struct sockaddr_un addr;
 
    /* try to delete the file if it already exists 
    Only throw an error if any potential error returned 
    by remove() is not ENOENT (no such file or directory)
    */   
    if (remove(mysock) == -1 && errno != ENOENT){
        puts("remove failed");
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (!sfd){
        puts("socket() failed");
        exit(EXIT_FAILURE);
    }

    // zero the contents of the  struct, to make sure it's completely clean
    memset(&addr, 0, sizeof(struct sockaddr_un));
    // configure the socket as appropriate
    addr.sun_family = AF_UNIX;
    // can't assign arrays; have to be copied manually
    // sizeof return the number of bytes that can be held in the array arg
    strncpy(addr.sun_path, mysock, sizeof(addr.sun_path) - 1); // use -i so that the last byte remains 0 (NUL)

    /* try to bind to the socket;
       sockaddr_un has to be cast to a sockaddr for the socket functions 
       to work on it 
    */
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1){
    puts("failed to bind");
    exit(EXIT_FAILURE);
    }
    else{
        puts("Listening ... \n");
    }
    // bytes received
    ssize_t numbytes=0;
    socklen_t len;

    /* message protocol */
    struct msg_protocol message;
    memset(&message, 0, sizeof(struct msg_protocol));
    // zero the contents of the inner struct arrays
    //memset(message.msg, 0, BUFF_SIZE);
    //memset(message.progname, 0, PROGNAME_LIMIT);
    
    // similar to while(true)
    for (Logger_log_start(&logger);;){
        len = sizeof(struct sockaddr);
        // read the bytes into the message struct
        numbytes = recvfrom(sfd, &message, sizeof(struct msg_protocol), 0, (struct sockaddr*)&addr, &len);
        if (numbytes == -1){
            puts("recvfrom error");
            exit(EXIT_FAILURE);
        }
        printf("received: %zu bytes\n", numbytes);
        printf("message.msg is : %s and msg.progname is : %s and loglevel is : %s and tag is : %s\n", message.msg, message.progname, Logger_loglevel_to_string(message.log_level), message.tag);
        
        Logger_log(&logger, message.progname, message.msg, message.tag, message.log_level);
    }
    Logger_log_end(&logger);
}





















