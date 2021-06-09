CC := g++
FLAGS += -g -DDEBUG -std=c++11

SRC_PATH := .
SRC := $(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.cpp))

#去掉c文件目录
SRC_WITHOUT_DIR:=$(notdir $(SRC))
#生成.o文件列表
OBJ_WITHOUT_DIR:=$(patsubst %.cpp,%.o,$(SRC_WITHOUT_DIR))
#为.o文件列表加上编译目录
OBJ_WITH_BUILD_DIR:=$(addprefix ./,$(OBJ_WITHOUT_DIR))
$(info "OBJ_WITH_BUILD_DIR:$(OBJ_WITH_BUILD_DIR)")
#为gcc添加源文件搜索目录
VPATH=$(SRC_PATH)

BIN := test

INCLUDE := -I. -I../rapidjson -I../protocol -I../include -I../util
LINKFLAGS := -Wl,-rpath=. -Wl,-rpath=../lib
LIBS := -L. -L ../lib -lprotocol -lutil

all : $(BIN)

$(BIN) : $(OBJ_WITH_BUILD_DIR)
	$(CC) $(FLAGS) $(LINKFLAGS) -o $@ $^ $(LIBS)

%.o : %.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@

.PHONY : clean
clean:
	-rm -f *.o
	-rm -f test


