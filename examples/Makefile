# steed example
PROJECT = SteedExample
LIBSTEED = steed
BINARY = $(PROJECT)

THIRDDIRS = ../src/thirdparty/cli11/include
SRCSUBDIR += ${THIRDDIRS}
SRCSUBDIR += ../src/steed/conf
SRCSUBDIR += ../src/steed/util
SRCSUBDIR += ../src/steed/base
SRCSUBDIR += ../src/steed/schema
SRCSUBDIR += ../src/steed/store
SRCSUBDIR += ../src/steed/assemble
SRCSUBDIR += ../src/steed/parse
SRCSUBDIR += ../src/steed/unittest
INCLUDES  = $(addprefix -I,$(SRCSUBDIR))

# compiler
CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror -Wall
CFLAGS += -g -O3 -fPIC
CFLAGS += -I../include $(INCLUDES)

# linker
LDFLAGS = -ldl -L/usr/local/lib -l$(LIBSTEED) 

# source files
SRCS = main.cpp

# executable file
$(BINARY): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# clean
clean:
	rm -rf $(BINARY) $(OBJS)
