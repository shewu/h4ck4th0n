CC=g++
LD=g++

CCFLAGS = -Wall

debug: CCFLAGS += -O0 -g -DDEBUG -Werror
all: CCFLAGS += -O2 -w

UNHOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
HOLY_LDFLAGS=-lOpenCL -lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
#HOLY_LDFLAGS=-lOpenCL -lSDL -lGL -lGLU -lalut -lopenal -lGLEW -L/opt/AMDAPP/lib/x86_64 -O2 -g

SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs
HOLY_BALLS_TARGET=holyballs
MULTI_BALLS_TARGET=multiballs

SHARED_OBJECTS = socket.o \
	Vector.o \
	Object.o \
	world.o \
	menu.o \
    HBMap.o \
	menuitem.o \
	menudraw.o \
	font.o \
	SplashViewController.o \
	ServerConnectViewController.o \
	GameViewController.o \
	packet.o \
	SocketConnection.o

SERVER_OBJECTS+=$(SHARED_OBJECTS) server.o game.o simulate.o
UNHOLY_BALLS_OBJECTS+=$(SHARED_OBJECTS) unholyclient.o UnholyGameViewController.o
HOLY_BALLS_OBJECTS+=$(SHARED_OBJECTS) holyclient.o HolyGameViewController.o
MULTI_BALLS_OBJECTS+=$(SHARED_OBJECTS) multiclient.o MultiGameViewController.o

executables: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET)

all: executables

debug: executables

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(LD) -o $(SERVER_TARGET) $(SERVER_LDFLAGS) $(SERVER_OBJECTS)

$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	$(LD) -o $(UNHOLY_BALLS_TARGET) $(UNHOLY_LDFLAGS) $(UNHOLY_BALLS_OBJECTS)

$(HOLY_BALLS_TARGET): $(HOLY_BALLS_OBJECTS)
	$(LD) -o $(HOLY_BALLS_TARGET) $(HOLY_LDFLAGS) $(HOLY_BALLS_OBJECTS)

$(MULTI_BALLS_TARGET): $(MULTI_BALLS_OBJECTS)
	$(LD) -o $(MULTI_BALLS_TARGET) $(HOLY_LDFLAGS) $(MULTI_BALLS_OBJECTS)

%.o: %.cpp *.h
	$(CC) -c $(CCFLAGS) $<

unholyclient.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -DUNHOLY -o unholyclient.o $<

holyclient.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -o holyclient.o $<

multiclient.o: client.cpp *.h
	$(CC) -c $(CCFLAGS) -DMULTI -o multiclient.o $<

test: $(SHARED_OBJECTS)
	(cd tests; make test)

test2: $(SHARED_OBJECTS)
	(cd tests; make test2)

clean:
	rm -rf $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET) *.o

love:
	echo 'In Soviet Russia, love makes you!'

