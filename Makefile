SERVER_SOURCES:=srv.c clogger.c demon.c
CLIENT_SOURCES:=client.c clogger.c

CC:=gcc
CCFLAGS:=-Wall -c -o

SERVER_PROGNAME:=cloggerd
CLIENT_PROGNAME:=clogger


.PHONY: all
# building "all" means building the server and then the client
all : build_server build_client done

build_server: $(SERVER_SOURCES:.c=.o) 
	@echo "\nBuilding server ... "
	$(CC) $^ -o $(SERVER_PROGNAME)


build_client: $(CLIENT_SOURCES:.c=.o) 
	@echo "\nBuilding client ..."
	$(CC) $^ -o $(CLIENT_PROGNAME)

done :
	@echo "\nDONE."

# no need for this, as it's built-in : rule 
# to build any .o object file from a .c source file
%.o : %.c
	$(CC) $^ $(CCFLAGS) $@

clean:
	@echo "\nCleaning artifacts ... "
	rm *.o $(SERVER_PROGNAME) $(CLIENT_PROGNAME)




