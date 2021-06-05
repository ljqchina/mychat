LIB_PATH := ../lib/
CFLAGS := -std=c++11
LFLAGS := -lrt
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,$(OBJ_DIR)%.o,$(SRCS))
TARGET := libprotocol.so

all : $(BIN_DIR)$(TARGET) 

INC_DIR := -I ../rapidjson

$(BIN_DIR)$(TARGET) : $(OBJS)
	g++ $(CFLAGS) $^ -shared -o $@ $(LFLAGS)

$(OBJ_DIR)%.o : %.cpp
	g++ $(CFLAGS) $(INC_DIR) -fPIC -c $< -o $@

.PHONY: install clean
install :
	#cp -f ./wipedisk.h $(INC_PATH)
	cp -f ./libprotocol.so $(LIB_PATH)
	#ldconfig

clean:
	rm -f *.o $(TARGET)

