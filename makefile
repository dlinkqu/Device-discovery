GCC = gcc
CP  = cp

MACHINE ?= $(shell uname -m)

CFLAGS = -Wall -Werror -g  -DLINUX -Wl,-rpath=./

LIB = -lpthread
INCLUDES = -I.

OBJ_SERVER = server.o list.o
OBJ_CLIENT = client.o


%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

all: clean server client
server: $(OBJ_SERVER)
	$(GCC) -o  server $(OBJ_SERVER) $(LDFLAGS) $(LIB)

client: $(OBJ_CLIENT)
	$(GCC) -o  server $(OBJ_CLIENT) $(LDFLAGS) $(LIB)
 
clean: 
	rm -f *.o
	rm -f server client
	
