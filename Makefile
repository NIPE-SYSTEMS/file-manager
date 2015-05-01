# Copyright (C) 2015 NIPE-SYSTEMS
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

CC = gcc

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += `pkg-config --cflags libsoup-2.4`
CFLAGS += `pkg-config --cflags json-glib-1.0`

LIBS += `pkg-config --libs libsoup-2.4`
LIBS += `pkg-config --libs json-glib-1.0`

.PHONY: all file-manager init clean

all: file-manager

# Compile main program

file-manager: init bin/obj/file-manager.o
	$(CC) bin/obj/file-manager.o $(CFLAGS) -o bin/file-manager $(LIBS)

# Initializes directories

init:
	mkdir -p bin
	mkdir -p bin/obj

# C-objects

bin/obj/file-manager.o: src/file-manager.c
	$(CC) $(CFLAGS) -c -o bin/obj/file-manager.o src/file-manager.c $(LIBS)

# Clean

clean:
	rm -Rf bin