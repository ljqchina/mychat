CC := g++
FLAGS += -g -DDEBUG -std=c++11

ROOT_DIR=$(shell pwd)
OBJ_DIR := $(ROOT_DIR)/tmp/obj
BIN_DIR := $(ROOT_DIR)/tmp/bin
export OBJ_DIR BIN_DIR

SUBDIRS := $(ROOT_DIR)/tcpsrv
SRC_PATH := $(ROOT_DIR) $(SUBDIRS)
SRC := $(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.cpp))

#去掉c文件目录
SRC_WITHOUT_DIR:=$(notdir $(SRC))
#生成.o文件列表
OBJ_WITHOUT_DIR:=$(patsubst %.cpp,%.o,$(SRC_WITHOUT_DIR))
#为.o文件列表加上编译目录
OBJ_WITH_BUILD_DIR:=$(addprefix $(OBJ_DIR)/,$(OBJ_WITHOUT_DIR))
$(info "OBJ_WITH_BUILD_DIR:$(OBJ_WITH_BUILD_DIR)")
#为gcc添加源文件搜索目录
VPATH=$(SRC_PATH)

BIN := mychat

INCLUDE := -I. -I ./tcpsrv -I ../include -I ../rapidjson -I ../protocol -I ./db
LINKFLAGS := -Wl,-rpath=. -Wl,-rpath=../lib
LIBS := -L. -L ../lib -levent -lprotocol -ldb

all : build_prepare $(BIN)

$(BIN) : $(OBJ_WITH_BUILD_DIR)
	$(CC) $(FLAGS) $(LINKFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@

#创建编译目录
build_prepare:
	@if [ ! -d $(OBJ_DIR) ]; then \
	mkdir -p $(OBJ_DIR); \
	mkdir -p $(BIN_DIR); \
	fi

.PHONY : clean
clean:
	-rm -f $(OBJ_DIR)/*
	-rm -f $(BIN_DIR)/*


