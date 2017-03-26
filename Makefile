CC=clang++
LD=clang++

OBJ_DIR = bin/objs
OUT_DIR = bin
DEP_DIR = bin/deps
SRC_DIR = src

CCSTD = c++0x

CCFLAGS = -Wall -MD -MP --std=$(CCSTD)

ifeq ($(DEBUG), 1)
	CCFLAGS += -O0 -g -DDEBUG
else
	CCFLAGS += -O2 -w
endif

UNHOLY_LDFLAGS=-lSDL2 -lGL -lGLU -lalut -lopenal -lGLEW
HOLY_LDFLAGS=-lOpenCL -lSDL2 -lGL -lGLU -lalut -lopenal -lGLEW
#HOLY_LDFLAGS=-lOpenCL -lSDL2 -lGL -lGLU -lalut -lopenal -lGLEW -L/opt/AMDAPP/lib/x86_64 -O2 -g

PLAYBACKSERVER_TARGET=playback
SERVER_TARGET=server
UNHOLY_BALLS_TARGET=unholyballs
HOLY_BALLS_TARGET=holyballs
MULTI_BALLS_TARGET=multiballs

SHARED_OBJECTS = Socket.o \
	Object.o \
	World.o \
    HBMap.o \
	Packet.o \
	SocketConnection.o \
	Material.o \
	MapElements.o

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

SERVER_OBJECTS=$(addprefix $(OBJ_DIR)/,$(SHARED_OBJECTS) Server.o Game.o Physics.o PhysicsWorld.o PhysicsForces.o CTFGame.o UserInput.o Logging.o)
PLAYBACKSERVER_OBJECTS=$(addprefix $(OBJ_DIR)/,$(SHARED_OBJECTS) Game.o Physics.o PhysicsWorld.o PhysicsForces.o CTFGame.o UserInput.o PlaybackServer.o)
UNHOLY_BALLS_OBJECTS=$(addprefix $(OBJ_DIR)/,$(CLIENT_OBJECTS) Unholyclient.o UnholyGameViewController.o)
HOLY_BALLS_OBJECTS=$(addprefix $(OBJ_DIR)/,$(CLIENT_OBJECTS) Holyclient.o HolyGameViewController.o)
MULTI_BALLS_OBJECTS=$(addprefix $(OBJ_DIR)/,$(CLIENT_OBJECTS) Multiclient.o MultiGameViewController.o)

EXECUTABLES=$(addprefix $(OUT_DIR)/,$(SERVER_TARGET) $(UNHOLY_BALLS_TARGET) $(HOLY_BALLS_TARGET) $(MULTI_BALLS_TARGET) $(PLAYBACKSERVER_TARGET))

-include Makefile-overrides

executables: $(EXECUTABLES)

all: executables

$(SERVER_TARGET) : $(OUT_DIR)/$(SERVER_TARGET)
$(PLAYBACKSERVER_TARGET) : $(OUT_DIR)/$(PLAYBACKSERVER_TARGET)
$(UNHOLY_BALLS_TARGET) : $(OUT_DIR)/$(UNHOLY_BALLS_TARGET)
$(HOLY_BALLS_TARGET) : $(OUT_DIR)/$(HOLY_BALLS_TARGET)
$(MULTI_BALLS_TARGET) : $(OUT_DIR)/$(MULTI_BALLS_TARGET)

$(OUT_DIR)/$(SERVER_TARGET): $(SERVER_OBJECTS)
	@mkdir -p $(OUT_DIR)
	$(LD) -o $(OUT_DIR)/$(SERVER_TARGET) $(SERVER_OBJECTS) $(SERVER_LDFLAGS)

$(OUT_DIR)/$(PLAYBACKSERVER_TARGET): $(PLAYBACKSERVER_OBJECTS)
	@mkdir -p $(OUT_DIR)
	$(LD) -o $(OUT_DIR)/$(PLAYBACKSERVER_TARGET) $(PLAYBACKSERVER_OBJECTS) $(SERVER_LDFLAGS)

$(OUT_DIR)/$(UNHOLY_BALLS_TARGET): $(UNHOLY_BALLS_OBJECTS)
	@mkdir -p $(OUT_DIR)
	$(LD) -o $(OUT_DIR)/$(UNHOLY_BALLS_TARGET) $(UNHOLY_BALLS_OBJECTS) $(UNHOLY_LDFLAGS)

$(OUT_DIR)/$(HOLY_BALLS_TARGET): $(HOLY_BALLS_OBJECTS)
	@mkdir -p $(OUT_DIR)
	$(LD) -o $(OUT_DIR)/$(HOLY_BALLS_TARGET) $(HOLY_BALLS_OBJECTS) $(HOLY_LDFLAGS)

$(OUT_DIR)/$(MULTI_BALLS_TARGET): $(MULTI_BALLS_OBJECTS)
	@mkdir -p $(OUT_DIR)
	$(LD) -o $(OUT_DIR)/$(MULTI_BALLS_TARGET) $(MULTI_BALLS_OBJECTS) $(HOLY_LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)/$(SRC_DIR)
	$(CC) -c $(CCFLAGS) -o $@ $< -MF "$(DEP_DIR)/$(<:.cpp=.d)"

$(OBJ_DIR)/Unholyclient.o: $(SRC_DIR)/Client.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)/$(SRC_DIR)
	$(CC) -c $(CCFLAGS) -DUNHOLY -o $@ $< -MF "$(DEP_DIR)/Unholyclient.d"

$(OBJ_DIR)/Holyclient.o: $(SRC_DIR)/Client.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)/$(SRC_DIR)
	$(CC) -c $(CCFLAGS) -o $@ $< -MF "$(DEP_DIR)/Holyclient.d"

$(OBJ_DIR)/Multiclient.o: $(SRC_DIR)/Client.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(DEP_DIR)/$(SRC_DIR)
	$(CC) -c $(CCFLAGS) -DMULTI -o $@ $< -MF "$(DEP_DIR)/Multiclient.d"

test: $(SHARED_OBJECTS)
	(cd tests; make test)

test2: $(SHARED_OBJECTS)
	(cd tests; make test2)

clean:
	@rm -rf $(EXECUTABLES) $(OUT_DIR)

love:
	@echo 'In Soviet Russia, love makes you!'

format:
	cd src && find \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 clang-format -style=file -i

# Include these files which contain all the dependencies
# between the source files and the header files, so if a header
# file changes, the correct things will re-compile.
# rwildcard is a helper to search recursively for .d files
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
-include $(call rwildcard,$(DEP_DIR)/,*.d)
