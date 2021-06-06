LIB_PATH := ../lib/
CFLAGS := -std=c++11 -g
LFLAGS := -lrt
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
TARGET := libdboper.so

all : $(BIN_DIR)$(TARGET) 

INC_DIR := -I ../rapidjson

$(BIN_DIR)$(TARGET) : $(OBJS)
	g++ $(CFLAGS) $^ -shared -o $@ $(LFLAGS)
	mv -f ./libdboper.so $(LIB_PATH)

$(OBJ_DIR)%.o : %.cpp
	g++ $(CFLAGS) $(INC_DIR) -fPIC -c $< -o $@

.PHONY: install clean
install :
	mv -f ./libdboper.so $(LIB_PATH)
	#ldconfig

clean:
	rm -f *.o $(TARGET)

