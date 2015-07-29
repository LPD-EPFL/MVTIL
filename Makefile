CFLAGS:=-std=c++11

ifeq ($(DEBUG),1)
  DEBUG_FLAGS=-Wall -ggdb -g -DDEBUG
  CFLAGS += -O0 -fno-inline
else
  DEBUG_FLAGS=-Wall
  CFLAGS += -O3
endif

CC        := g++
LD        := g++

MODULES   := common server tests
SRC_DIR   := $(MODULES)
BUILD_DIR := $(addprefix build/,$(MODULES))


SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ       :=  $(patsubst %.cc,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cc $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/test_server

build/test_server: $(OBJ)
	$(LD) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
