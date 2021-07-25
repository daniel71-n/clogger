#ifndef DEMON_H
#define DEMON_H
#include <stdlib.h>


/* Daemonize a process; 
 * Returns -1 on error and 0 on success 
 */
ssize_t daemonize(void);

#endif
