DEBUG ?= n
VERBOSE ?= n

SRC_DIR = ./src
PUBSUB_DIR = $(SRC_DIR)/pubsub_cpp
BUILD_DIR = ./build

#
# pub
#
PUB_SRCS += \
	$(PUBSUB_DIR)/publisher.cpp \
	$(SRC_DIR)/pub.cpp \

PUB_OBJS := $(subst $(SRC_DIR),$(BUILD_DIR),$(patsubst %.cpp,%.o,$(PUB_SRCS)))

#
# sub
#
SUB_SRCS += \
	$(PUBSUB_DIR)/subscriber.cpp \
	$(SRC_DIR)/sub.cpp \

SUB_OBJS := $(subst $(SRC_DIR),$(BUILD_DIR),$(patsubst %.cpp,%.o,$(SUB_SRCS)))

#
# record
#
RECORD_SRCS += \
	$(PUBSUB_DIR)/subscriber.cpp \
	$(SRC_DIR)/record.cpp \

RECORD_OBJS := $(subst $(SRC_DIR),$(BUILD_DIR),$(patsubst %.cpp,%.o,$(RECORD_SRCS)))

#
# playback
#
PLAYBACK_SRCS += \
	$(PUBSUB_DIR)/publisher.cpp \
	$(SRC_DIR)/playback.cpp \

PLAYBACK_OBJS := $(subst $(SRC_DIR),$(BUILD_DIR),$(patsubst %.cpp,%.o,$(PLAYBACK_SRCS)))

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

all: pub sub record playback

pub: $(PUB_OBJS)
	$(LD) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

sub: $(SUB_OBJS)
	$(LD) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

record: $(RECORD_OBJS)
	$(LD) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

playback: $(PLAYBACK_OBJS)
	$(LD) -o $(BUILD_DIR)/$@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CPPFLAGS)

clean:
	@rm -rf $(BUILD_DIR)
