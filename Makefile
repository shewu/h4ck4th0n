CC=g++
LD=g++

CCFLAGS = -Wall

ifeq ($(DEBUG), 1)
	CCFLAGS += --std=c++11 -O0 -g -DDEBUG -Werror
else
	CCFLAGS += --std=c++11 -O2 -w
endif

UNHOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW
HOLY_LDFLAGS=-lOpenCL -lSDL -lGL -lGLU -lalut -lopenal -lGLEW
#HOLY_LDFLAGS=-lOpenCL -lSDL -lGL -lGLU -lalut -lopenal -lGLEW -L/opt/AMDAPP/lib/x86_64 -O2 -g

SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs
HOLY_BALLS_TARGET=holyballs
MULTI_BALLS_TARGET=multiballs

SHARED_OBJECTS = Socket.o \
	Vector.o \
	Object.o \
	World.o \
    HBMap.o \
	Packet.o \
	SocketConnection.o \
	Material.o

CLIENT_OBJECTS = $(SHARED_OBJECTS) \
    Menu.o \
	MenuItem.o \
	MenuDraw.o \
	Font.o \
	SplashViewController.o \
	ServerConnectViewController.o \
	GameViewController.o \
	MirroringWorld.o \
	UserInput.o

SERVER_OBJECTS+=$(SHARED_OBJECTS) Server.o Game.o Physics.o PhysicsWorld.o Forces.o CTFGame.o UserInput.o
UNHOLY_BALLS_OBJECTS+=$(CLIENT_OBJECTS) Unholyclient.o UnholyGameViewController.o
HOLY_BALLS_OBJECTS+=$(CLIENT_OBJECTS) Holyclient.o HolyGameViewController.o
MULTI_BALLS_OBJECTS+=$(CLIENT_OBJECTS) Multiclient.o MultiGameViewController.o

executables: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET)

include Makefile-overrides

all: executables

$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(LD) -o $(SERVER_TARGET) $(SERVER_OBJECTS) $(SERVER_LDFLAGS)

$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	$(LD) -o $(UNHOLY_BALLS_TARGET) $(UNHOLY_BALLS_OBJECTS) $(UNHOLY_LDFLAGS)

$(HOLY_BALLS_TARGET): $(HOLY_BALLS_OBJECTS)
	$(LD) -o $(HOLY_BALLS_TARGET) $(HOLY_BALLS_OBJECTS) $(HOLY_LDFLAGS)

$(MULTI_BALLS_TARGET): $(MULTI_BALLS_OBJECTS)
	$(LD) -o $(MULTI_BALLS_TARGET) $(MULTI_BALLS_OBJECTS) $(HOLY_LDFLAGS)

%.o: %.cpp *.h
	$(CC) -c $(CCFLAGS) $<

Unholyclient.o: Client.cpp *.h
	$(CC) -c $(CCFLAGS) -DUNHOLY -o Unholyclient.o $<

Holyclient.o: Client.cpp *.h
	$(CC) -c $(CCFLAGS) -o Holyclient.o $<

Multiclient.o: Client.cpp *.h
	$(CC) -c $(CCFLAGS) -DMULTI -o Multiclient.o $<

test: $(SHARED_OBJECTS)
	(cd tests; make test)

test2: $(SHARED_OBJECTS)
	(cd tests; make test2)

clean:
	rm -rf $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET) *.o

love:
	echo 'In Soviet Russia, love makes you!'

