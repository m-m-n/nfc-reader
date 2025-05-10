CC = gcc
CFLAGS = -Wall -Wextra -I/usr/include/PCSC -D_GNU_SOURCE
LDFLAGS = -lpcsclite

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/nfc-reader
SRCS = $(SRC_DIR)/nfc_reader.c $(SRC_DIR)/card_list.c $(SRC_DIR)/pcsc_utils.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	install -m 600 /dev/null /usr/local/etc/allowed-list.txt
