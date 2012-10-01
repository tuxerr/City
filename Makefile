# output executable name
EXEC = city
# source files
CXX = g++
CXXFLAGS  = -Wall -W -O2 -g -Wunused -I./inc -I./tools -DUSE_GLEW -std=c++0x 
LDFLAGS = -lm -lrt -lGLEW -lGL -lGLU -lsfml-graphics -lsfml-window -lsfml-system -ltinyxml -lpthread -L./tools -Wl -lSOIL
SRCS = $(wildcard src/*.cpp)
INCS = $(wildcard inc/*.hpp)
OBJS = $(SRCS:.cpp=.o)

all: $(EXEC)

$(EXEC) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o : %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean :
	rm -rf $(OBJS)
