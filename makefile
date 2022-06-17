CC=g++
# CCFLAGS=-Wall  -std=c++11 -g -pg
# LDFLAGS=  -std=c++11 -g  -pg
CCFLAGS=-Wall -O3 -std=c++11
LDFLAGS=-O3 -std=c++11 -lboost_system -lboost_iostreams -lboost_filesystem -lpthread

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

SOURCES := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)
OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.o)
DEBUG_OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.d.o)

TARGET=stickersolve

# all: $(TARGET)
all: release

gprof:
	gprof $(TARGET)_DEBUG gmon.out  >output.txt

release: $(TARGET)
#	./$(TARGET)

debug: $(TARGET)_DEBUG
# 	gdb $(TARGET)_DEBUG
#	./$(TARGET)_DEBUG

$(TARGET)_DEBUG: $(DEBUG_OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) -ggdb -pg

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@
	
$(BUILD_DIR)/%.cpp.d.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@ -ggdb -pg

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TARGET)_DEBUG
	
MKDIR_P ?= mkdir -p
