CC = clang++

SRC = src
BUILD = build

_OBJS = bhrg.cpp entity.cpp
OBJS = $(patsubst %,$(SRC)/%,$(_OBJS))

INCLUDE_FLAGS = -I$(SRC)/entity.hpp -I/usr/local/include/essentia/ -I/usr/local/include/essentia/scheduler/ -I/usr/local/include/essentia/streaming/ -I/usr/local/include/essentia/utils -I/usr/local/include/essentia
LINKER_FLAGS = -lSDL2 -lSDL2_mixer -L/usr/local/lib -lessentia -lfftw3 -lavcodec -lavformat -lavutil -lavresample -lsamplerate -ltag -lfftw3f
OBJ_NAME = bhrg

all: $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o $(BUILD)/$(OBJ_NAME)

run: $(BUILD)/$(OBJ_NAME)
	$(BUILD)/$(OBJ_NAME)

.PHONY: clean

clean:
	rm -r $(BUILD)/*
