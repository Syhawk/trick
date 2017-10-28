.PHONY:clean all
CC=gcc
CXX=g++
CFLAGS=-W -g
CXXFLAGS=-W -g
BIN=posix_create posix_delete posix_getattr posix_send posix_recv posix_notify

all:$(BIN)
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
posix_create:posix_create.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_delete:posix_delete.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_getattr:posix_getattr.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_send:posix_send.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_recv:posix_recv.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_notify:posix_notify.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt

clean:
	rm -f *.o $(BIN)
