DEBUG ?= n
VERBOSE ?= n

SRC_DIR = ./src
PUBSUB_DIR = $(SRC_DIR)/pubsub_cpp
BUILD_DIR = ./build

#
# sub
#
SUB_SRCS += \
	$(PUBSUB_DIR)/subscriber.cpp \
	$(SRC_DIR)/sub.cpp \

SUB_OBJS := $(subst $(SRC_DIR),$(BUILD_DIR),$(patsubst %.cpp,%.o,$(SUB_SRCS)))

#
# Recipes
#
DEPS := $(shell find $(SRC_DIR) -name "*.h")

INC_DIRS += \
	$(SRC_DIR) \
	/usr/local/include \

CPPFLAGS += $(addprefix -I, $(INC_DIRS))
CPPFLAGS += -O0
CPPFLAGS += -Wall
CPPFLAGS += -Werror
ifeq ($(DEBUG),y)
CPPFLAGS += -g
endif

LDFLAGS +=

ifeq ($(VERBOSE),y)
CC = g++
LD = g++
else
CC = @echo '   ' CC $@; g++
LD = @echo '   ' LD $@; g++
endif

all: sub

sub: $(SUB_OBJS)
	$(LD) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CPPFLAGS)

clean:
	@rm -rf $(BUILD_DIR)
