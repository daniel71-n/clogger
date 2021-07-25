This is a client-server setup that uses the clogger module I wrote 
to log messages passed to it at the command line. 

The way this works is similar to syslogd, though it's obviously more limited
in scope and functionality.

- there's a daemon listening on a UNIX DOMAIN SOCKET for requests. 
- The client sends messages through that socket.
- the server receives them, and simply writes them to a logfile 
using clogger.

Note that clogger can be used independently of this setup. 
Here it's merely part of it, responsible for actually writing and formatting 
the log messages.

Here's a quick summary of the files involved:
-> client.c // a client written in C that you can use to send messages to the daemon for logging
-> srv.c // the actual daemon listening on the UNIX DOMAIN SOCKET for messages to log
->clogger.c // the actual logger implementation, used by the daemon to log messages
->clogger.h // header file used only by clogger.c
->udslog.h // header file used by the C client and daemon
->client.lua // alternative lua client to send messages to the C daemon
