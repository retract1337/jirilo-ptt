CC = clang

TARGET = ptt
SRC_DIR = src
INC_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INC_DIR)/*.h)

CFLAGS = -Wall -Wextra -I$(INC_DIR)  
LDFLAGS = 

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)

