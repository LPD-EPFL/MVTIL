CFLAGS:=-std=c++14

ifeq ($(DEBUG),1)
  DEBUG_FLAGS=-Wall -ggdb -g -DDEBUG
  CFLAGS += -O0 -fno-inline -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
else
  DEBUG_FLAGS=-Wall 
  CFLAGS += -O3 -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
endif

CC        := g++
LD        := g++

LIBS := -lpthread -lthrift -lthriftnb -levent -lcityhash 
LIB_DIR := -L/usr/local/lib -L/home/jwang/lib 

FILTER_FILE := gen-cpp/OracleServer_server.skeleton.cpp gen-cpp/DataServer_server.skeleton.cpp

#server
MODULES_SERVER   := common server gen-cpp
SRC_DIR_SERVER   := $(MODULES_SERVER)
BUILD_DIR_SERVER := $(addprefix build/,$(MODULES_SERVER))

SRC_SERVER       := $(foreach sdir,$(SRC_DIR_SERVER),$(filter-out $FILTER_FILE, $(wildcard $(sdir)/*.cpp)))
OBJ_SERVER       :=  $(patsubst %.cpp,build/%.o,$(SRC_SERVER))
INCLUDES_SERVER  := $(addprefix -I,$(SRC_DIR_SERVER))

INCLUDES_SERVER += -I/usr/local/include -I/home/jwang/include

#client
MODULES_CLIENT   := common client gen-cpp
SRC_DIR_CLIENT   := $(MODULES_CLIENT)
BUILD_DIR_CLIENT := $(addprefix build/,$(MODULES_CLIENT))

SRC_CLIENT       := $(foreach sdir,$(SRC_DIR_CLIENT),$(filter-out $FILTER_FILE, $(wildcard $(sdir)/*.cpp)))
OBJ_CLIENT       :=  $(patsubst %.cpp,build/%.o,$(SRC_CLIENT))
INCLUDES_CLIENT  := $(addprefix -I,$(SRC_DIR_CLIENT))

INCLUDES_CLIENT += -I/usr/local/include -I/home/jwang/include


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

all: checkdirs thrift build/parse build/server_exec build/server_exec2 build/client_exec build/test_single_client build/test_multi_client build/performance_key_space 

#build/performance_single build/performance_multi 
#build/performance_scale_key_space
build/parse: $(OBJ_SERVER) $(OBJ_CLIENT) build/parse.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/parse.o: tests/parse.cpp
	$(CC) $(INCLUDES_SERVER) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#server
build/server_exec: $(OBJ_SERVER) build/server_exec.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/server_exec.o: tests/server_exec.cpp
	$(CC) $(INCLUDES_SERVER) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/server_exec2: $(OBJ_SERVER) build/server_exec2.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/server_exec2.o: tests/server_exec2.cpp
	$(CC) $(INCLUDES_SERVER) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#client
# build/client_exec: $(OBJ_CLIENT) build/performance.o
# 	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ -L/usr/local/lib $(LIBS)

build/client_exec: $(OBJ_CLIENT) build/client_exec.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/client_exec.o: tests/client_exec.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/test_single_client: $(OBJ_CLIENT) build/test_single_client.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR)$ $(LIBS)

build/test_single_client.o: tests/test_single_client.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/test_multi_client: $(OBJ_CLIENT) build/test_multi_client.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/test_multi_client.o: tests/test_multi_client.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#build/performance_single: $(OBJ_CLIENT) build/performance_single.o
#	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

#build/performance_single.o: tests/performance_single.cpp
#	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#build/performance_multi: $(OBJ_CLIENT) build/performance_multi.o
#	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

#build/performance_multi.o: tests/performance_multi.cpp
#	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

build/performance_key_space: $(OBJ_CLIENT) build/performance_key_space.o
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

build/performance_key_space.o: tests/performance_key_space.cpp
	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

#build/performance_scale_key_space: $(OBJ_CLIENT) build/performance_scale_key_space.o
#	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@ $(LIB_DIR) $(LIBS)

#build/performance_scale_key_space.o: tests/performance_scale_key_space.cpp
#	$(CC) $(INCLUDES_CLIENT) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

checkdirs: $(BUILD_DIR_ALL)

$(BUILD_DIR_ALL):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR_SERVER) $(BUILD_DIR_CLIENT)

$(foreach bdir,$(BUILD_DIR_SERVER),$(eval $(call make-goal-server,$(bdir))))

$(foreach bdir,$(BUILD_DIR_CLIENT),$(eval $(call make-goal-client,$(bdir))))

DATA_SERVICE:=DataService

#thrift: gen-cpp/$(DATA_SERVICE)_types.cpp

#gen-cpp/$(DATA_SERVICE)_types.cpp: thrift/$(DATA_SERVICE).thrift
#	thrift -r --gen cpp $<
