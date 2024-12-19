
CC = clang
TARGET = ptt
SRC_DIR = src
INC_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INC_DIR)/*.h)

CFLAGS = -Wall -Wextra -I$(INC_DIR)

DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += $(DEBUG_FLAGS)
else
    CFLAGS += $(RELEASE_FLAGS)
endif

LDFLAGS = 

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)

