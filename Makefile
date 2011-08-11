# The lines of interest to you are SERVER_OBJECTS and UNHOLY_BALLS_OBJECTS, which specify the object files that are required for the server and unholyballs, respectively

ifeq ($(DEBUG_MODE),on)
 DEBUG_FLAGS=-DDEBUG
else
 DEBUG_FLAGS=
endif

CC=g++
LD=g++

#CCFLAGS=-O2 -g $(DEBUG_FLAGS)
CCFLAGS=-O2 -g -DDEBUG
UNHOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
HOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -lIL -lOpenCL -O2 -g

SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs

SERVER_OBJECTS=clcomm.o world.o socket.o object.o vec.o simulate.o
UNHOLY_BALLS_OBJECTS=client.o socket.o vec.o object.o world.o menu.o menuitem.o menudraw.o font.o SplashViewController.o ServerConnectViewController.o GameViewController.o

all: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(LD) -o $(SERVER_TARGET) $(SERVER_LDFLAGS) $(SERVER_OBJECTS)

$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	$(LD) -o $(UNHOLY_BALLS_TARGET) $(UNHOLY_LDFLAGS) $(UNHOLY_BALLS_OBJECTS)

%.o: %.cpp *.h
	$(CC) -c $(CCFLAGS) $<

unholy.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -DUNHOLY -o unholy.o $<

clean:
	rm -rf $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(SERVER_OBJECTS) $(UNHOLY_BALLS_OBJECTS)

love:
	echo 'In Soviet Russia, love makes you!'

macserver:
	g++ -O2 -g clcomm.cpp world.cpp socket.cpp object.cpp vec.cpp simulate.cpp SDLMain.m -I/Library/Frameworks/SDL.framework/Headers -framework SDL -framework Cocoa -o macserver

mac:
	g++ -O2 client.cpp font.cpp menu.cpp menudraw.cpp menuitem.cpp object.cpp unholyrender.cpp socket.cpp vec.cpp world.cpp SDLMain.m -framework SDL -framework OpenGL -framework Cocoa -framework GLUT -I/Library/Frameworks/SDL.framework/Headers -o unholyballs

