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

LIBS := -lthrift -lcityhash

#server
MODULES_SERVER   := common server gen-cpp
SRC_DIR_SERVER   := $(MODULES_SERVER)
BUILD_DIR_SERVER := $(addprefix build/,$(MODULES_SERVER))

SRC_SERVER       := $(foreach sdir,$(SRC_DIR_SERVER),$(filter-out gen-cpp/DataServer_server.skeleton.cpp, $(wildcard $(sdir)/*.cpp)))
OBJ_SERVER       :=  $(patsubst %.cpp,build/%.o,$(SRC_SERVER))
INCLUDES_SERVER  := $(addprefix -I,$(SRC_DIR_SERVER))

INCLUDES_SERVER += -I/usr/local/include/thrift -I/usr/local/include

#client
MODULES_CLIENT   := common client gen-cpp
SRC_DIR_CLIENT   := $(MODULES_CLIENT)
BUILD_DIR_CLIENT := $(addprefix build/,$(MODULES_CLIENT))

SRC_CLIENT       := $(foreach sdir,$(SRC_DIR_CLIENT),$(filter-out gen-cpp/DataServer_server.skeleton.cpp, $(wildcard $(sdir)/*.cpp)))
OBJ_CLIENT       :=  $(patsubst %.cpp,build/%.o,$(SRC_CLIENT))
INCLUDES_CLIENT  := $(addprefix -I,$(SRC_DIR_CLIENT))

INCLUDES_CLIENT += -I/usr/local/include/thrift -I/usr/local/include


ALL_MODULES :=  $(sort $(MODULES_SERVER) $(MODULES_CLIENT))
BUILD_DIR_ALL := $(addprefix build/,$(ALL_MODULES))


vpath %.cpp $(SRC_DIR_SERVER) $(SRC_DIR_CLIENT)

define make-goal-server
$1/%.o: %.cpp
	$(CC) $(INCLUDES_SERVER) $(CFLAGS) $(DEBUG_FLAGS) -c $$< -o $$@
endef

define make-goal-client
$1/%.o: %.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs thrift clean

all: checkdirs thrift build/server_exec build/client_exec

#server
build/server_exec: $(OBJ_SERVER) build/server_exec.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ -L/usr/local/lib $(LIBS)

build/server_exec.o: tests/server_exec.cpp
	$(CC) $(INCLUDES_SERVER) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#client
build/client_exec: $(OBJ_CLIENT) build/performance.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ -L/usr/local/lib $(LIBS)

build/client_exec.o: tests/client_exec.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/performance.o: tests/performance.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@
checkdirs: $(BUILD_DIR_ALL)

$(BUILD_DIR_ALL):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR_SERVER) $(BUILD_DIR_CLIENT)

$(foreach bdir,$(BUILD_DIR_SERVER),$(eval $(call make-goal-server,$(bdir))))

$(foreach bdir,$(BUILD_DIR_CLIENT),$(eval $(call make-goal-client,$(bdir))))

THRIFT_SERVICE:=OracleService

thrift: gen-cpp/$(THRIFT_SERVICE)_types.cpp

gen-cpp/$(THRIFT_SERVICE)_types.cpp: thrift/$(THRIFT_SERVICE).thrift
	thrift -r --gen cpp $<

