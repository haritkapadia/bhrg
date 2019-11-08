CC = clang++

SRC = src
BUILD = build

_OBJS = bhrg.cpp
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

INCLUDE_FLAGS = 
LINKER_FLAGS = -lSDL2 -lSDL2_mixer
OBJ_NAME = bhrg

all: $(SRC)/$(OBJS)
	$(CC) $(OBJS) $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o $(BUILD)/$(OBJ_NAME)

run: $(BUILD)/$(OBJ_NAME)
	$(BUILD)/$(OBJ_NAME)

.PHONY: clean

clean:
	rm -r $(BUILD)/*
