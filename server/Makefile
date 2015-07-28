ifeq ($(DEBUG),1)
	DEBUG_FLAGS=-Wall -ggdb -DDEBUG
	COMPILE_FLAGS=-O0 -fno-inline
else
	DEBUG_FLAGS=-Wall
	COMPILE_FLAGS=-O3
endif

LIBS:=-lrt -lpthread
CXX:=g++

TARGET=server
TOP := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

COMPILE_FLAGS += -D_GNU_SOURCE -std=c++11
INCLUDES:=-I$(TOP) -I$(TOP)/../common
#OBJDIR:=$(TOP)/../objs

#OBJS=$(join $(addsuffix ../obj/, $(dir $(TARGET))), $(notdir $(SOURCE:.cpp=.o))) 

#COMMON_SRC:=LockSet.cpp

VPATH:=$(TOP)/../common

#COMMON_OBJS := $(patsubst %.cpp, $(OBJDIR)/%$(ARCH)$(DEBUG).o, $(notdir $(COMMON_SRC)))

$(ObjDir)/%$(ARCH)$(DEBUG).o : %.cpp Makefile
    @echo creating $@ ...
    @$(CXX) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

.PHONY: all clean

#%.o:: $(SRC)/%.c 

%.o : %.cpp
        @mkdir -p $(dir $@)
        @echo "============="
        @echo "Compiling $<"
		@$(GXX) $(COMPLIE_FLAGS) $(INCLUDES) -o $@ -c $<
        #@$(CC) $(CFLAGS) -c $< -o $@

server: text_server.c
		$(GXX) $(COMPLIE_FLAGS) $(INCLUDES) -o $@ -c $<

all:

clean:
	rm -f *.o
