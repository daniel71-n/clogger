#include <assert.h>

#include "clogger.h"

// string counterparts for each Log_level enum member
char *Log_level_strings[4] = {"INFO", "NOTICE", "WARNING", "FATAL"};

// indexes Log_level_strings to get the string represenation for each log level enum  member
const char *Logger_loglevel_to_string(enum Log_levels level){
    //printf("level is %s\n", Log_level_strings[level]);
    return Log_level_strings[level];
}


static void Logger_empty_start_end_function(struct Logger *logger_ptr){
    return;
}

static void Logger_empty_function(struct Logger *logger_ptr, const char source[], const char message[], char *tag, enum Log_levels level){ 
    return;
}

/* gets the current time and formats it into a string, and then places as state inside the 
   the Logger object 
*/
static void Logger_get_datetime(struct Logger *logger_ptr){
    time_t raw_time= time(NULL);
    if (raw_time == -1){
        puts("Time function failed.\n");
        exit(EXIT_FAILURE);
    }
    logger_ptr->broken_down_time = localtime(&raw_time);
    if (!logger_ptr->broken_down_time){
        puts("Time function failed.\n");
        exit(EXIT_FAILURE);
    }
    // e.g. 14:03:22 3 Apr 2019
    strftime(logger_ptr->datetime, sizeof(logger_ptr->datetime), "%b %d %Y %H:%M:%S", logger_ptr->broken_down_time);
}


void Logger_log(struct Logger* logger_ptr, const char source[], const char message[], char *tag, enum Log_levels level){
    size_t res = 0; // used to test return codes
     
    //don't log unless the log level of the message is equal in priority to or greaten than the
    //current Logger loglevel'
    if (! (level >= logger_ptr->level)){
        return; 
    }
    else{
        Logger_get_datetime(logger_ptr);
        //struct tm *time_struct = logger_ptr->broken_down_time;
        /*fprintf(logger_ptr->handle, " | %i:%i:%i %i/%i/%i : %s\n", time_struct->tm_hour, time_struct->tm_min, time_struct->tm_sec, \
                time_struct->tm_mday, time_struct->tm_mon, time_struct->tm_year, message);
        */
        
        // format of the message is "date_time : ~LOG_LEVEL : < TAG > : [ SOURCE ] : MESSAGE"
        // tag can be empty, in which case, - is printed out instead e.g. < - >
        res = fprintf(logger_ptr->handle, " | %s : ~%-7s : <%s> : [%s] : %s\n", logger_ptr->datetime, Logger_loglevel_to_string(level), tag, source, message);
        if (res < 0){
            puts("fprintf failure: failed to write to file\n");
            exit(EXIT_FAILURE);
        }
        fflush(logger_ptr->handle); // flush the fd; otherwise if there's a crash, it might not get written
    }
}


void Logger_log_start(struct Logger *logger_ptr){
    size_t res = 0;
    char hr[] = " | =============================================\n";
    char message[] = " * * * Start Logging * * *";
    Logger_get_datetime(logger_ptr);
    //struct tm *time_struct = logger_ptr->broken_down_time;
    res = fputs(hr, logger_ptr->handle);
    if (res == EOF){
        puts("fputs failure");
        exit(EXIT_FAILURE);
    }
    res = fprintf(logger_ptr->handle, " | %s : %s\n", logger_ptr->datetime, message);
    if (res < 0){
        puts("fprintf failure");
        exit(EXIT_FAILURE);
    }
    fflush(logger_ptr->handle); // flush the fd; otherwise if there's a crash, it might not get written
}

void Logger_log_end(struct Logger *logger_ptr){
    char hr[] = " | =============================================\n";
    char message[] = " * * * Stop Logging * * * ";
    Logger_get_datetime(logger_ptr);
    fprintf(logger_ptr->handle, " | %s : %s\n", logger_ptr->datetime, message);
    fputs(hr, logger_ptr->handle);
    fflush(logger_ptr->handle); // flush the fd; otherwise if there's a crash, it might not get written
}



void Logger_init(struct Logger *logger, const char file_handle[], bool enabled){
    if (!(strcmp(file_handle, "stdout"))){
        logger->handle=stdout; //stdout is typr
    }
    else{
        FILE *fh=NULL;
        if ((fh = fopen(file_handle, "a")) == NULL){
            printf("Error opening file %s\n", file_handle);
            exit(EXIT_FAILURE);
        }
        logger->handle = fh;
    }
    
    logger->enabled=enabled;
    /* if logger is not enabled, simply use the empty function versions, 
       which don't do anything */
    if (!enabled){
        logger->log = &Logger_empty_function;
        logger->log_start = &Logger_empty_start_end_function;
        logger->log_end = &Logger_empty_start_end_function;
    }
    else{
        logger->log = &Logger_log;
        logger->log_start = &Logger_log_start;
        logger->log_end = &Logger_log_end;
    }
}

/* set the level of the logging. Only messages passed with level smaller
   or equal to this (via Logger_log() will get written to the logfile)
*/
void Logger_set_level(struct Logger *logger_ptr, enum Log_levels level){
    logger_ptr->level=level;

}

/* test that the log level provided as a string is valid;
   exit with error if not, else return the enum type
*/
enum Log_levels validate_loglevel(const char *log_level){
    assert(log_level);

    if (strcmp(log_level, "log_notice") == 0){
        return LOG_NOTICE;
    }

    else if(strcmp(log_level, "log_info") == 0){
        return LOG_INFO;
    }

    else if (strcmp(log_level, "log_warning") == 0){
        return LOG_WARNING;
    }

    else if (strcmp(log_level, "log_fatal") == 0){
        return LOG_FATAL;
    }

    else{
        perror("Invalid log level value");
        exit(EXIT_FAILURE);
    }

}


















