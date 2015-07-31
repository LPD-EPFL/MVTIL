CFLAGS:=-std=c++11

ifeq ($(DEBUG),1)
  DEBUG_FLAGS=-Wall -ggdb -g -DDEBUG
  CFLAGS += -O0 -fno-inline -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
else
  DEBUG_FLAGS=-Wall 
  CFLAGS += -O3 -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
endif

CC        := g++
LD        := g++

LIBS := -lthrift

MODULES   := common server thrift/gen-cpp
SRC_DIR   := $(MODULES)
BUILD_DIR := $(addprefix build/,$(MODULES))


SRC       := $(foreach sdir,$(SRC_DIR),$(filter-out thrift/gen-cpp/DataServer_server.skeleton.cpp, $(wildcard $(sdir)/*.cpp)))
OBJ       :=  $(patsubst %.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

INCLUDES += -I/usr/local/include/thrift -I/usr/local/include

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/server

build/server: $(OBJ) build/server.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ -L/usr/local/lib $(LIBS)

build/server.o: tests/server.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

thrift/gen-cpp/DataServer_types.cpp: thrift/DataServer.thrift
	thrift -r --gen cpp $<

