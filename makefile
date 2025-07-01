###############################################################
# Makefile for Logger project
# This makefile is used to build the Logger 
# library and the test binaries in sequential
# order. The makefile is divided into the
# following sections:
# 1. Directories
# 2. Compiler flags
# 3. Static libraries building flags
# 4. Libraries and LD flags
# 5. Files to compile libraries
# 6. Files to compile tests
# 7. Libs
# 8. Test binaries
# 9. Make libraries
# 10. Make tests
###############################################################

##Define various directories for the project
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
TEST_OBJ_DIR := $(OBJ_DIR)/test
BIN_DIR := bin
LIB_DIR := lib
TEST_DIR := tests

##Compiler flags
CXX := clang++

CXXFLAGS := -std=c++20 -g -Wall -Wextra -Werror -Wno-unused-function \
			-I$(INC_DIR) $(addprefix -I, $(wildcard $(INC_DIR)/*))

CXXFLAGS_TEST := -std=c++20 -g -Wall -Wextra -Werror -Wno-unused-function \
			-I$(INC_DIR) -I$(TEST_DIR) \
			$(addprefix -I, $(wildcard $(INC_DIR)/*), $(wildcard $(TEST_DIR)/*))

##Static libraries building flags
AR_FLAGS := ar
R_FLAGS := -rcs

##Libraries and LD flags used for linking test binaries
LIB_NAME := liblogger
DBG_LIB_NAME := liblogger_d
LD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(LIB_NAME))
LDD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(DBG_LIB_NAME))

##Files and variables to compile libraries
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DBG_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%_d.o, $(SRCS))

##Files and variables to compile tests
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp")
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/test/%.o, $(TEST_SRCS))
DBG_TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/test/%_d.o, $(TEST_SRCS))

##Library target names
TARGET := $(LIB_DIR)/$(LIB_NAME).a
DBG_TARGET := $(LIB_DIR)/$(DBG_LIB_NAME).a

##Test binary target names
TEST_TARGET := $(BIN_DIR)/TestLogger
TEST_DBG_TARGET := $(BIN_DIR)/TestLogger_d

all : debug release

release : $(TARGET) $(TEST_TARGET)	##Build release version of the library first and then the test release binary

debug : $(DBG_TARGET) $(TEST_DBG_TARGET)  ##Build debug version of the library first and then the test debug binary

##Make libraries
$(TARGET) : $(OBJS)
	@echo "Linking release build...."
	$(AR_FLAGS) $(R_FLAGS) $@ $^
	@echo "Linking release build completed"

$(DBG_TARGET) : $(DBG_OBJS) | $(LIB_DIR)
	@echo "Linking debug build...."
	$(AR_FLAGS) $(R_FLAGS) $@ $^
	@echo "Linking debug build completed"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling release build...."
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiling release build completed"

$(OBJ_DIR)/%_d.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling debug build...."
	$(CXX) $(CXXFLAGS) -c $< -o $@ -DDEBUG
	@echo "Compiling debug build completed"

##Make tests
$(TEST_TARGET) : $(TEST_OBJS) | $(BIN_DIR)
	@echo "Linking release test build...."
	$(CXX) $(CXXFLAGS_TEST) $^ -lgtest -lpthread $(LD_FLAGS) -o $@
	@echo "Linking release test build completed"

$(TEST_DBG_TARGET) : $(DBG_TEST_OBJS) | $(BIN_DIR)
	@echo "Linking debug test build...."
	$(CXX) $(CXXFLAGS_TEST) $^ -lgtest -lpthread $(LDD_FLAGS) -o $@
	@echo "Linking debug test build completed"

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJ_DIR)
	@echo "Compiling test release build...."
	$(CXX) $(CXXFLAGS_TEST) -c $< -o $@
	@echo "Compiling test release build completed"

$(TEST_OBJ_DIR)/%_d.o: $(TEST_DIR)/%.cpp | $(TEST_OBJ_DIR)
	@echo "Compiling debug test build...."
	$(CXX) $(CXXFLAGS_TEST) -c $< -o $@ -DDEBUG
	@echo "Compiling debug test build completed"

##Create directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TEST_OBJ_DIR):
	mkdir -p $(OBJ_DIR)/test

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

##Clean the solution
clean:
	@echo "Cleaning solution..."
	rm -rf $(OBJ_DIR) $(TEST_OBJ_DIR) \
		$(LIB_DIR) $(BIN_DIR) \
		$(TARGET) $(DBG_TARGET) \
		$(TEST_TARGET) $(TEST_DBG_TARGET)
	@echo "Cleaning solution completed"

.PHONY: all release debug clean
