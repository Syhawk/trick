.PHONY:clean all
CC=gcc
CXX=g++
CFLAGS=-W -g
CXXFLAGS=-W -g
BIN=posix_thread posix_thread_pc_problem_sem posix_thread_pc_problem_cond

all:$(BIN)
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
posix_thread:posix_thread.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread
posix_thread_pc_problem_sem:posix_thread_pc_problem_sem.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread
posix_thread_pc_problem_cond:posix_thread_pc_problem_cond.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

clean:
	rm -f *.o $(BIN)
