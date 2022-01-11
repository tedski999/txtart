# txtart - Simple ncurses ASCII art editor
# Copyright (C) 2022 Ted Johnson <tedjohnsonjs@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

TARGET    := txtart
VERSION   := $(shell git describe --abbrev=4 --dirty --always --tags)
CC        := gcc
SRC_FILES := txtart.c
CFLAGS    := -Wall -pedantic -O3 -DTXTART_VERSION=\"$(VERSION)\"
LFLAGS    := -lncurses

$(TARGET): $(SRC_FILES)
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(TARGET)
