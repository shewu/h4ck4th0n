CC=g++
LD=g++

CCFLAGS = -Wall

ifeq ($(DEBUG), 1)
	CCFLAGS += -O0 -g -DDEBUG -Werror
else
	CCFLAGS += -O2 -w
endif

UNHOLY_LDFLAGS=-lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
HOLY_LDFLAGS=-lOpenCL -lSDL -lGL -lGLU -lalut -lopenal -lGLEW -O2 -g
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
	SocketConnection.o

CLIENT_OBJECTS = Menu.o \
	MenuItem.o \
	MenuDraw.o \
	Font.o \
	SplashViewController.o \
	ServerConnectViewController.o \
	GameViewController.o

SERVER_OBJECTS+=$(SHARED_OBJECTS) Server.o Game.o Physics.o
UNHOLY_BALLS_OBJECTS+=$(SHARED_OBJECTS) Unholyclient.o UnholyGameViewController.o
HOLY_BALLS_OBJECTS+=$(SHARED_OBJECTS) Holyclient.o HolyGameViewController.o
MULTI_BALLS_OBJECTS+=$(SHARED_OBJECTS) Multiclient.o MultiGameViewController.o

executables: $(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET)

all: executables

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

