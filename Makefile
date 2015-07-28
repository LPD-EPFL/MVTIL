CC        := g++
LD        := g++

CFLAGS:=-std=c++11 -Wall

MODULES   := common server tests
SRC_DIR   := $(MODULES)
BUILD_DIR := $(addprefix build/,$(MODULES))


SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ       :=  $(patsubst %.cc,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cc $(SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs build/test_server

build/test_server: $(OBJ)
	$(LD) $(CFLAGS) $^ -o $@


checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
