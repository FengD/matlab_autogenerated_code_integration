INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

DIRS = $(shell find $(SRC_DIR) -maxdepth 4 -type d)

SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
OBJ = ${patsubst %.cpp, $(OBJ_DIR)/%.o, ${SRC}}

TARGET = libSimulinkSDK.a
LIB_TARGET = ${LIB_DIR}/${TARGET}

CXX ?= g++
CXXFLAGS = -pipe -g -Wall -I${INCLUDE_DIR} -c -O3 -mavx -mfma
STD14 = -std=c++14
LIB = -lpthread -lm -lrt

${LIB_TARGET}: ${OBJ}
	ar cru ${LIB_TARGET} ${OBJ} ${LIB_TARGET}

${OBJ_DIR}/%.o:	%.cpp
	${CXX} ${CXXFLAGS} $< -o  $@ ${STD14} ${LIB}

clean:
	find ${OBJ_DIR} -name *.o -exec rm -rf {} \;
	find ${LIB_DIR} -name *.a -exec rm -rf {} \;
