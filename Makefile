# The lines of interest to you are SERVER_OBJECTS and UNHOLY_BALLS_OBJECTS, which specify the object files that are required for the server and unholyballs, respectively

ifeq ($(DEBUG_MODE),on)
 DEBUG_FLAGS=-DDEBUG
else
 DEBUG_FLAGS=
endif

CC=g++
LD=g++

CCFLAGS=-O2 -g $(DEBUG_FLAGS)
UNHOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
HOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -lOpenCL -O2 -g

SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs
HOLY_BALLS_TARGET=holyballs

SERVER_OBJECTS=server.o game.o world.o socket.o object.o vec.o simulate.o
UNHOLY_BALLS_OBJECTS=client.o socket.o vec.o object.o world.o menu.o menuitem.o menudraw.o font.o SplashViewController.o ServerConnectViewController.o UnholyGameViewController.o
HOLY_BALLS_OBJECTS=client.o socket.o vec.o object.o world.o menu.o menuitem.o menudraw.o font.o SplashViewController.o ServerConnectViewController.o HolyGameViewController.o

all: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(LD) -o $(SERVER_TARGET) $(SERVER_LDFLAGS) $(SERVER_OBJECTS)

$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	$(LD) -o $(UNHOLY_BALLS_TARGET) $(UNHOLY_LDFLAGS) $(UNHOLY_BALLS_OBJECTS)

$(HOLY_BALLS_TARGET): $(HOLY_BALLS_OBJECTS)
	$(LD) -o $(HOLY_BALLS_TARGET) $(HOLY_LDFLAGS) $(HOLY_BALLS_OBJECTS)

%.o: %.cpp *.h
	$(CC) -c $(CCFLAGS) $<

unholy.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -DUNHOLY -o unholy.o $<

holy.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -o holy.o $<

UnholyGameViewController.o: GameViewController.cpp *.h
	$(CC) -c $(CCFLAGS) -DUNHOLY -o UnholyGameViewController.o $<

HolyGameViewController.o: GameViewController.cpp *.h
	$(CC) -c $(CCFLAGS) -o HolyGameViewController.o $<

clean:
	rm -rf $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(SERVER_OBJECTS) $(UNHOLY_BALLS_OBJECTS) $(HOLY_BALLS_TARGET) $(HOLY_BALLS_OBJECTS)

love:
	echo 'In Soviet Russia, love makes you!'

