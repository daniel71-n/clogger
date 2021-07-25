
#ifndef CLOGGER_H
#define CLOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

enum Log_levels{LOG_INFO, LOG_NOTICE, LOG_WARNING, LOG_FATAL};

/* Logger object holding state. Passed around by most functions */
struct Logger{
    FILE *handle;
    unsigned short level;
    bool enabled;
    char datetime[70]; // should be plenty
    struct tm *broken_down_time; // value writen by the localtime() function
    // the functions below get selected dynamically, according to whether the logger
    // is enabled or not ('enabled' is set to true or false)
    void (*log)(struct Logger *logger_ptr, const char source[], const char message[], char *tag, enum Log_levels level);
    void (*log_start)(struct Logger *logger_ptr);
    void (*log_end)();

};

// translates a Log_levels enum member to a string for inclusion in other strings
const char *Logger_loglevel_to_string(enum Log_levels level);

/* Log a message;
 * Args are: 
 *     - logger_ptr : a pointer to the logger object instantiated
 *     - source : a string representing the source of the logged message, normally the
 *     name of the program that sent the message
 *     - tag : optional string providing a tag, to make searching through the logs easier
 *     - message: the message to be logged
 *     - level : the level of the log; this will be compared against the logger object's
 *     set log level and only if it's smaller or equal to this, will the message be
 *     logged.
 */
void Logger_log(struct Logger* logger_ptr, const char source[], const char message[], char *tag, enum Log_levels level);

// writes an initial log message to the logs to signal the start of the session
void Logger_log_start(struct Logger *logger_ptr);
// writes a final log message to the logs to signal the end of the session
void Logger_log_end(struct Logger *logger_ptr);

// initializes a Logger object with the right state
void Logger_init(struct Logger *logger, const char file_handle[], bool enabled);

// sets the current log level; this affects what actually gets written to the logs
void Logger_set_level(struct Logger *logger_ptr, enum Log_levels level);

// provided a string meant to represent a loglevel, this function returns a Log_level
// enum, if valid, or -1 if invalid
enum Log_levels validate_loglevel(const char * log_level);

/* ==============================================================================
 * ||||||||||||||||||||||||||||||||| * USAGE * ||||||||||||||||||||||||||||||||||
 * ==============================================================================
 *
 * -------------------> Initializing a logger object <----------------------------
 * Declare a Logger struct; pointers to this are then passed to all the functions 
 * that take it as an argument eg Logger_init(), which will take care of everything.
 * Along with a pointer to this struct, also pass a path to a file to be used as the file 
 * handle to write the logs to.
 * the enabled flag turns the logger on an off; that is, if you set enabled to false
 * when initializing the logger object, then nothing will be printed to the logfile.
 *
 * The file handle can be NULL, in which case, stdout will be used.
 *
 * ---------------> setting the logging level <-------------------------------
 * When using the logger to log a message, you also pass an argument denoting the level 
 * of the message; this is a Log_level enum.
 * when you set the level of the logger with Logger_set_level, it makes it so that
 * only those messages with 'level' lower than or equal to it get logged (see, from left to right,
 * the Log_levels enum definition at the top of this file). 
 * The rest of them get disregarded.
 *
 * -------------------> logging a message <-------------------------------------
 * To write a message to the logfile, you use Logger_log. the argument this takes are
 * a pointer to a Logger struct already initialized by log_init(), a message string,
 * and a level, as discussed above.
 *
 * --------------------->Logger_log_Start() and Logger_log_end() <----------------------------
 * The point of these is that the messages printed will look different to signal the beginning
 * and end of the log. So log_Start should be run before any call to Logger_log(), and log_end()
 * should be called last, after all the calls to logger_log() to mark the end.
 *
 *
 *
 *
 */

#endif

