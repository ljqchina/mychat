LIB_PATH := ../lib/
CFLAGS := -std=c++11 -g
LFLAGS := -lrt
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
TARGET := libprotocol.so

all : $(BIN_DIR)$(TARGET) 

INC_DIR := -I ../rapidjson -I ../include

$(BIN_DIR)$(TARGET) : $(OBJS)
	g++ $(CFLAGS) $^ -shared -o $@ $(LFLAGS)
	mv -f ./libprotocol.so $(LIB_PATH)

$(OBJ_DIR)%.o : %.cpp
	g++ $(CFLAGS) $(INC_DIR) -fPIC -c $< -o $@

.PHONY: install clean
install :
	#cp -f ./Protocol.h $(INC_PATH)
	mv -f ./libprotocol.so $(LIB_PATH)
	#ldconfig

clean:
	rm -f *.o $(TARGET)

