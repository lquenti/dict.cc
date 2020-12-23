B_DIR=build

S_FILES=dict.cc
EXEC=$(B_DIR)/dictcc

CC=g++
C_FLAGS=--std=c++17 -O2
L_FLAGS=-lcurl

all: build

build:
	$(CC) $(S_FILES) $(C_FLAGS) $(L_FLAGS) -o $(EXEC)

clean:
	rm -rf $(B_DIR)/*

# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all build clean
