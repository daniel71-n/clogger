#ifndef UDS_CLOGGER_H
#define UDS_CLOGGER_H

// maximum length of the message
#define BUFF_SIZE 500
// maximum program name in the struct below
#define PROGNAME_LIMIT 100
// maximum tag length
#define TAG_SIZE 50 

#include "clogger.h"

// default socket path; override with -s option
#define SOCK_PATH "/tmp/clogger.sock"
// default logfile path; override with -s option
#define LOG_PATH "/tmp/clog.log"

/*the message the client is supposed to send is a struct of this type;
  the server reads the size of this struct into the structure, 
  then uses it to construct a message for the logger. 
*/
struct msg_protocol{
    char progname[PROGNAME_LIMIT]; // the source of the message being logged: normally the current program name, but it could be any arbitrary string
    char msg[BUFF_SIZE]; // the message proper that needs logging
    char tag[TAG_SIZE];  // a tag to more easily be able to grep for your message in the logs; supposed to be a single word
    enum Log_levels log_level; // pass the log level of the current message to the logger as well
};


#endif
