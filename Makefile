CC := g++
FLAGS += -g -DDEBUG -std=c++11

MODNAME := $(lastword $(subst /, ,$(dir $(abspath $(MAKEFILE_LIST)))))
ROOT_DIR=$(shell pwd)
OBJ_DIR := $(ROOT_DIR)/tmp/obj
BIN_DIR := $(ROOT_DIR)/tmp/bin
export OBJ_DIR BIN_DIR

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(notdir $(SRCS)))

TARGETS := mychat 
BINS := $(TARGETS)

INCLUDE := -I. -I ./tcpsrv
LINKFLAGS := -Wl,-rpath=. -Wl,-rpath=./lib
LIBS := -L. -L ./lib -levent

SUBDIRS := tcpsrv

all : $(SUBDIRS) $(BINS)
$(SUBDIRS) : ECHO
	make -C $@
ECHO :
	@echo $(SUBDIRS)

$(BINS) : $(OBJ_DIR)/*.o
	$(CC) $(FLAGS) $(LINKFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

%.o : %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $(OBJ_DIR)/$@

.PHONY : clean
clean:
	-rm -f $(OBJ_DIR)/*
	-rm -f $(BIN_DIR)/*


