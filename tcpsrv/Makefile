INC_PATH := #/usr/local/include/
LIB_PATH := ../lib/
CFLAGS := -std=c++11
LFLAGS := -lrt
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all : $(OBJS) 
$(OBJS) : %.o : %.cpp
	g++ $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

.PHONY: install clean

clean:
	rm -f *.o $(TARGET)

