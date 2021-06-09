LIB_PATH := ../lib/
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
TARGET := libutil.so

CFLAGS := -std=c++11 -g
LFLAGS := -lrt -lcrypto

all : $(BIN_DIR)$(TARGET) 

INC_DIR := -I.

$(BIN_DIR)$(TARGET) : $(OBJS)
	g++ $(CFLAGS) $^ -shared -o $@ $(LFLAGS)
	mv -f ./$(TARGET) $(LIB_PATH)

$(OBJ_DIR)%.o : %.cpp
	g++ $(CFLAGS) $(INC_DIR) -fPIC -c $< -o $@

.PHONY: install clean
install :
	mv -f ./$(TARGET) $(LIB_PATH)
	#ldconfig

clean:
	rm -f *.o $(TARGET) *.gch

