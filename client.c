#include <stdio.h>
#include <strings.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "clogger.h"
#include "udslog.h"


// the level of logging for sent messages; this is part of the message sent to the logging
// server; this is the default -- can be overwritten with the -l option
#define LOGLEVEL LOG_INFO;


void usage(void);
void check_not_null(void *arg);


int main(int argc, char *argv[]){

    struct sockaddr_un srvaddr;     // server address socket object
    int sfd;                        // socket file descriptor -- returned by connect()
    size_t msglen = 0;                  // the length of the message sent
    size_t numbytes = 0;                // numbytes returned

    // for optarg; used to store theg current char option
    ssize_t opt = 0;

    // path to unix socket; use the default -- overridden by -s arg if provided
    char *mysock=SOCK_PATH;


    // this is the struct that will be sent to the logger through the socket
    // zero the contents
    struct msg_protocol message;
    // zero the struct
    memset(&message, 0, sizeof(struct msg_protocol));
    message.log_level = LOGLEVEL;
    //printf("client side: loglevel is %s\n", Logger_loglevel_to_string(message.log_level));

    // set progname in the message to argv[0] if nothing has been explicitly provided
    strncpy(message.progname, argv[0], PROGNAME_LIMIT-1);

    
    //parse args with getopt
    while ((opt = getopt(argc, argv, "!s:hm:l:p:t:")) != -1){
        switch(opt){

            case 'h':
                // print usage message
                usage();
                exit(EXIT_SUCCESS); // don't continue, but exit, if -h is used

            case 's':
                // override the path to the socket
                mysock = optarg;
                break;

            // message to log
            case 'm':
                check_not_null(optarg);
                strncpy(message.msg, optarg, BUFF_SIZE-1);
                break;

            case 'l':
                check_not_null(optarg);
                message.log_level = validate_loglevel(optarg);
                break;

           // non-standard program name to use for the message source in the logs 
            case 'p':
                // -1 so that the terminating NUl does not get overwritten
                strncpy(message.progname, optarg, PROGNAME_LIMIT-1);
                break;

            case 't':
                // optional tag for the message
                strncpy(message.tag, optarg, TAG_SIZE-1);
                break;

            case '?':
                fprintf(stderr, "Unknown option : %c\n", optopt);
                break;
        }
    }
    

    //create client socket
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1){
        puts("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //construct server address
    memset(&srvaddr, 0, sizeof(struct sockaddr_un));
    srvaddr.sun_family = AF_UNIX;
    strncpy(srvaddr.sun_path, mysock, sizeof(srvaddr.sun_path)- 1);
 
    //msglen = strlen(argv[2]) + 1; // strlen doesn't count the terminating NUL
    msglen = sizeof(struct msg_protocol);
    numbytes = sendto(sfd, &message, msglen, 0, (struct sockaddr *) &srvaddr, sizeof(struct sockaddr_un));
    printf("strlen is %zu and numbytes is %zi\n", msglen, numbytes);

    if (numbytes != msglen){
        puts("failed to send whole message");
        exit(EXIT_FAILURE);
    }
}










/* print usage message when the wrong option is provided or the prog is called with -h */
void usage(void){
char *usage_string = "\
\nUSAGE: \n\
\t -h \t print this help message and exit \n\
\t -s \t path to unix domain socket that the server is listening on (override the default one) \n\
\t -m \t message to log : maximum 500 chars \n\
\t -t \t optional tag for the message, to more easily find it in the logs \n\
\t -l \t log level \n\
\t -p \t optional alternative program name to record as the source of the message in the logs \n\n\
";
printf("%s", usage_string);
printf("The defaults are as follows: \n\t log level : %s \n\t log file : %s \n\t socket path : %s \n\n", "LOG_INFO", LOG_PATH, SOCK_PATH); 
printf("Possible values for the log level provided through -l: \n \
    * log_info \n \
    * log_warning \n \
    * log_notice \n \
    * log_ftal \n\n"); 
}


// quickly check that the argument provided is NOT null
void check_not_null(void *arg){
    if (!arg){
        perror("NULL argument provided");
        exit(EXIT_FAILURE);
    }
}












