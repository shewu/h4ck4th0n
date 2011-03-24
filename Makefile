# The lines of interest to you are SERVER_OBJECTS and UNHOLY_BALLS_OBJECTS, which specify the object files that are required for the server and unholyballs, respectively

CC=g++
LD=g++
CCFLAGS=-O2 -g
LDFLAGS=-lSDL -lGL -lGLU -lalut -lGLEW -lIL -O2 -g
SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs
SERVER_OBJECTS=clcomm.o world.o socket.o object.o vec.o simulate.o
UNHOLY_BALLS_OBJECTS=unholyballs.o unholyrender.o socket.o vec.o object.o world.o

all: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(LD) -o $(SERVER_TARGET) $(LDFLAGS) $(SERVER_OBJECTS)

$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	$(LD) -o $(UNHOLY_BALLS_TARGET) $(LDFLAGS) $(UNHOLY_BALLS_OBJECTS)

%.o: %.cpp *.h
	$(CC) -c $(CCFLAGS) $<

clean:
	rm -rf $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(SERVER_OBJECTS) $(UNHOLY_BALLS_OBJECTS)

