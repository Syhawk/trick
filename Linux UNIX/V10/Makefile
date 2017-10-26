.PHONY:clean all
CC=gcc
CXX=g++
CFLAGS=-Wall -g
CXXFLAGS=-Wall -g
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
BIN=shmfifo_send shmfifo_recv shmfifo_destroy
OBJS1=shmfifo_send.o sem_operator.o shmfifo.o
OBJS2=shmfifo_recv.o sem_operator.o shmfifo.o
OBJS3=shmfifo_destroy.o sem_operator.o shmfifo.o

shmfifo_send:$(OBJS1)
	$(CXX) $(CXXFLAGS) $^ -o $@

shmfifo_recv:$(OBJS2)
	$(CXX) $(CXXFLAGS) $^ -o $@

shmfifo_destroy:$(OBJS3)
	$(CXX) $(CXXFLAGS) $^ -o $@

all:$(OBJS)
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN)
