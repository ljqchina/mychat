CC := g++
FLAGS += -g -DDEBUG -std=c++11

MODNAME := $(lastword $(subst /, ,$(dir $(abspath $(MAKEFILE_LIST)))))
OBJ_DIR := .
BIN_DIR := .

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

TARGETS := tcpsrv
BINS := $(addprefix $(BIN_DIR)/,$(TARGETS))

INCLUDE := -I. 
LINKFLAGS := -Wl,-rpath=.
LIBS := -L. -levent

all : $(BINS)

$(BINS) : $(OBJS)
	$(CC) $(FLAGS) $(LINKFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@

.PHONY : clean
clean:
	-rm -r $(OBJS) $(BINS)

