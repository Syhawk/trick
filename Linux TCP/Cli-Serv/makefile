.PHONY:clean all

OUTPUT_DIR := output
CXX = g++
CXXFLAGS = -W -g
SRCS := $(wildcard src/*.cpp)
MAIN := $(wildcard main/*.cpp)
DEPS := $(SRCS:.cpp=.d)
SRCS_OBJS := $(SRCS:.cpp=.o)
MAIN_OBJS := $(MAIN:.cpp=.o)
OBJS := $(SRCS_OBJS) $(MAIN_OBJS)
OUTPUT_SRCS_OBJS := $(addprefix $(OUTPUT_DIR)/, $(SRCS_OBJS))
OUTPUT_MAIN_OBJS := $(addprefix $(OUTPUT_DIR)/, $(MAIN_OBJS))
OUTPUT_OBJS := $(OUTPUT_SRCS_OBJS) $(OUTPUT_MAIN_OBJS)
OUTPUT_BINS := $(OUTPUT_MAIN_OBJS:.o=)
INC := -I ./include


all: bins
bins: $(OUTPUT_BINS)
%: %.o
	$(CXX) $(CXXFLAGS) $(OUTPUT_SRCS_OBJS) $^ -o $@
$(OUTPUT_OBJS): $(OBJS)
%.o: %.cpp
	@ if test ! -d $(OUTPUT_DIR)/$(dir $@); \
	then \
		mkdir -p $(OUTPUT_DIR)/$(dir $@); \
	fi; \
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $(OUTPUT_DIR)/$@
deps: $(DEPS)
%.d: %.cpp
	@ set -e; rm -f $(OUTPUT_DIR)/$@; echo $@; \
	if test ! -d $(OUTPUT_DIR)/$(dir $@); \
	then \
		mkdir -p $(OUTPUT_DIR)/$(dir $@); \
	fi; \
	$(CXX) -MM $(CXXFLAGS) $(INC) $< > $@.$$$$; \
	sed 's,\($*\)\.[ :]*,\1.o $@,g' < $@.$$$$ > $(OUTPUT_DIR)/$@; \
	rm -f $@.$$$$

clean:
	-rm -rf $(OUTPUT_DIR)
