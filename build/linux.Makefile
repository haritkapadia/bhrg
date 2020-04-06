SRC = src
BUILD = build

OBJECTS = bhrg.o mapmaker.o camera.o entity.o map.o world.o vec.o region.o stopwatch.o timeline.o effect.o spell.o event.o

# you might need -lSDL2main at the front
LINKER_FLAGS = -lSDL2 -lSDL2_mixer -lSDL2_ttf

CXX = g++
CXXFLAGS = -ggdb

vpath %.cpp ../src
vpath %.hpp ../src

bhrg : bhrg.o camera.o entity.o map.o world.o vec.o region.o stopwatch.o timeline.o effect.o spell.o event.o
	$(CXX) $^ $(CXXFLAGS) $(LINKER_FLAGS) -o bhrg

mapmaker : mapmaker.o camera.o entity.o map.o world.o vec.o region.o stopwatch.o timeline.o effect.o spell.o event.o
	$(CXX) $^ $(CXXFLAGS) $(LINKER_FLAGS) -o mapmaker

.PHONY : clean
clean :
	rm bhrg mapmaker $(OBJECTS)



bhrg.o: bhrg.cpp camera.hpp region.hpp vec.hpp \
  world.hpp effect.hpp entity.hpp event.hpp map.hpp timeline.hpp \
  stopwatch.hpp debug.hpp prettyprint.hpp spell.hpp
camera.o: camera.cpp camera.hpp region.hpp vec.hpp world.hpp effect.hpp \
  entity.hpp event.hpp map.hpp timeline.hpp stopwatch.hpp
effect.o: effect.cpp effect.hpp entity.hpp region.hpp vec.hpp event.hpp
entity.o: entity.cpp entity.hpp region.hpp vec.hpp
event.o: event.cpp event.hpp
map.o: map.cpp map.hpp region.hpp vec.hpp debug.hpp prettyprint.hpp
mapmaker.o: mapmaker.cpp \
  camera.hpp region.hpp vec.hpp world.hpp effect.hpp entity.hpp \
  event.hpp map.hpp timeline.hpp stopwatch.hpp debug.hpp prettyprint.hpp \
  spell.hpp
region.o: region.cpp debug.hpp prettyprint.hpp \
  region.hpp vec.hpp
spell.o: spell.cpp spell.hpp effect.hpp entity.hpp region.hpp vec.hpp \
  event.hpp timeline.hpp stopwatch.hpp world.hpp map.hpp debug.hpp \
  prettyprint.hpp
stopwatch.o: stopwatch.cpp stopwatch.hpp
timeline.o: timeline.cpp timeline.hpp entity.hpp region.hpp vec.hpp \
  event.hpp stopwatch.hpp debug.hpp prettyprint.hpp \
  world.hpp \
  effect.hpp map.hpp
vec.o: vec.cpp vec.hpp
world.o: world.cpp world.hpp effect.hpp entity.hpp region.hpp vec.hpp \
  event.hpp map.hpp timeline.hpp stopwatch.hpp debug.hpp prettyprint.hpp \
