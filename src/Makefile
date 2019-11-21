CXX = clang++

SRC = src
BUILD = build

OBJECTS = bhrg.o bounds.o camera.o entity.o spell.o world.o

ESSENTIA = /usr/local/include/essentia

INCLUDE_FLAGS = -I$(SRC)/class_util.hpp -I$(ESSENTIA)/scheduler/ -I$(ESSENTIA)/streaming/ -I$(ESSENTIA)/utils -I$(ESSENTIA)

LINKER_FLAGS = -lSDL2 -lSDL2_mixer -L/usr/local/lib -lessentia -lfftw3 -lavcodec -lavformat -lavutil -lavresample -lsamplerate -ltag -lfftw3f

CXXFLAGS = $(INCLUDE_FLAGS) $(LINKER_FLAGS)


bhrg : $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) --verbose -o $(BUILD)/bhrg

mapmaker : src/mapmaker.cpp src/entity.cpp src/bounds.cpp src/world.cpp src/camera.cpp
	$(CXX) src/mapmaker.cpp src/entity.cpp src/bounds.cpp src/world.cpp src/camera.cpp -lSDL2 -o $(BUILD)/mapmaker

bhrg.o : bounds.hpp entity.hpp world.hpp spell.hpp camera.hpp
# bounds.o : class_util.hpp
camera.o : bounds.hpp world.hpp
entity.o : bounds.hpp class_util.hpp
spell.o : world.hpp bounds.hpp entity.hpp
world.o : entity.hpp bounds.hpp

.PHONY : clean
clean :
	rm bhrg $(OBJECTS)

