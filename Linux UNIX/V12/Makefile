.PHONY:clean all
CC=gcc
CXX=g++
CFLAGS=-W -g
CXXFLAGS=-W -g
BIN=posix_shm_create posix_shm_delete posix_shm_write posix_shm_read

all:$(BIN)
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
posix_shm_create:posix_shm_create.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_shm_delete:posix_shm_delete.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_shm_write:posix_shm_write.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt
posix_shm_read:posix_shm_read.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lrt

clean:
	rm -f *.o $(BIN)
