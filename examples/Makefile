# steed example
PROJECT = SteedExample
LIBSTEED = steed
BINARY = $(PROJECT)

INSTALLDIR = /usr/local
STEEDHEAD = ${INSTALLDIR}/include/steed
THIRDDIRS = ../src/thirdparty/cli11/include
INCLUDES = -I$(STEEDHEAD) -I$(THIRDDIRS)

# compiler
CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror -Wall
CFLAGS += -g -O3 -fPIC
CFLAGS += $(INCLUDES)

# linker
LDFLAGS = -ldl -L${INSTALLDIR}/lib -l$(LIBSTEED)

# source files
SRCS = main.cpp

# executable file
$(BINARY): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# clean
clean:
	rm -rf $(BINARY) $(OBJS)
