# Makefile template for shared library
 
CC = gcc # C compiler
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I../.. # C flags
LDFLAGS = -shared  # linking flags
RM = rm -f  # rm command
TARGET_LIB = LibAvContextHandle.so # target lib
 
SRCS = LibAvContextHandle.cpp # source files
OBJS = $(SRCS:.c=.o)
 
.PHONY: all
all: ${TARGET_LIB}
 
$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^
 
$(SRCS:.c=.d):%.d:%.cpp
	$(CC) $(CFLAGS) -MM $< >$@
 
include $(SRCS:.c=.d)
 
.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)