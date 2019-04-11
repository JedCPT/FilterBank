
CXX = g++
SOURCE_FILES = filterbank.cpp header.cpp 

CFLAGS = -Wall -std=c++11 -DTIME
LIBS = 
INCLUDES = -Iinclude

SRC_DIR := src
BUILD_DIR := build
TEST_DIR := test
OBJS := $(SOURCE_FILES:.cpp=.o)

SRCS := $(addprefix $(SRC_DIR)/, $(SOURCE_FILES))
OBJS := $(addprefix $(BUILD_DIR)/, $(OBJS))

make: $(OBJS) $(SRCS)
	$(CXX) -c $(CFLAGS) $(LIBS) $(INCLUDES) $(OBJS) 

example: $(OBJS) $(SRCS)
	$(CXX) $(CFLAGS) $(LIBS) $(INCLUDES) $(OBJS) $(SRC_DIR)/main.cpp -o example 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(LIBS) $(INCLUDES) -c $<  -o $@


clean:
	rm build/* example
