.PHONY:clean all
CC=gcc
CXX=g++
CFLAGS=-W -g
CXXFLAGS=-W -g
BIN=posix_threadpool posix_condition threadpool_test
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
OBJS1=posix_threadpool.o posix_condition.o threadpool_test.o

all:$(OBJS)
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
threadpool_test:$(OBJS1)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lpthread -lrt

clean:
	rm -f *.o $(BIN)
