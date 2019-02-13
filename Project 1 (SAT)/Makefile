CXX = g++
CXXFLAGS = -std=c++11 -Wall -pedantic-errors -g

SRCS =  main.cpp
OBJS = ${SRCS:.cpp=.o}

MAIN = SAT

all: ${MAIN}

${MAIN}: ${OBJS}
		${CXX} ${CXXFLAGS} ${OBJS} -o ${MAIN}

