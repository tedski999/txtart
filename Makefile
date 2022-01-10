TARGET    := txtart
VERSION   := $(git describe --abbrev=4 --dirty --always --tags)
CC        := gcc
SRC_FILES := txtart.c
CFLAGS    := -Wall -pedantic -O3 -DTXTART_VERSION=\"$(VERSION)\"
LFLAGS    := -lcurses

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET)
