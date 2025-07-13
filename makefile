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

##Conditional variables for the makefile
BUILD_TYPE ?= release
BUILD_TEST ?= no
LIB_TYPE ?= static

##Compiler flags
CXX := clang++

CXXFLAGS := -std=c++20 -g -Wall -Wextra -Werror -Wno-unused-function -Wpedantic\
			-I$(INC_DIR) $(addprefix -I, $(wildcard $(INC_DIR)/*))

CXXFLAGS_TEST := -std=c++20 -g -Wall -Wextra -Werror -Wno-unused-function -Wpedantic\
			-I$(INC_DIR) -I$(TEST_DIR) \
			$(addprefix -I, $(wildcard $(INC_DIR)/*), $(wildcard $(TEST_DIR)/*))

##Static libraries building flags
AR_FLAGS := ar
R_FLAGS := -rcs

##Shared libraries building flags
FPIC_FLAG := -fPIC
SHARED_FLAG := -shared

##Libraries and LD flags used for linking test binaries
LIB_NAME := liblogger
DBG_LIB_NAME := liblogger_d

ifeq ($(LIB_TYPE), static)	# If it has to be a static lib
LD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(LIB_NAME))
LDD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(DBG_LIB_NAME))

else ifeq ($(LIB_TYPE), shared)	# If it has to be a shared lib
LD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(LIB_NAME)) -Wl,-rpath,$(LIB_DIR)
LDD_FLAGS := -L$(LIB_DIR) -l$(subst lib,,$(DBG_LIB_NAME)) -Wl,-rpath,$(LIB_DIR)
endif

##Files and variables to compile libraries
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DBG_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%_d.o, $(SRCS))

##Files and variables to compile tests
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp")
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/test/%.o, $(TEST_SRCS))
DBG_TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/test/%_d.o, $(TEST_SRCS))

##Library target names for making static lib
TARGET := $(LIB_DIR)/$(LIB_NAME).a
DBG_TARGET := $(LIB_DIR)/$(DBG_LIB_NAME).a

##Library target names for making shared lib
SHARED_TARGET := $(LIB_DIR)/$(LIB_NAME).so
SHARED_DBG_TARGET := $(LIB_DIR)/$(DBG_LIB_NAME).so

##Test binary target names
TEST_TARGET := $(BIN_DIR)/TestLogger
TEST_DBG_TARGET := $(BIN_DIR)/TestLogger_d

ifeq ($(BUILD_TYPE), release)
all: release	##Build release version of the library only

else ifeq ($(BUILD_TYPE), debug)
all: debug  ##Build debug version of the library only

else ifeq ($(BUILD_TYPE), all)
all: debug release  ##Build both debug and release versions of the library
endif

## If we have to only build the library not the test app
ifeq ($(BUILD_TEST), no)

ifeq ($(LIB_TYPE), static)
release : $(TARGET)
debug : $(DBG_TARGET)
else ifeq ($(LIB_TYPE), shared)
release : $(SHARED_TARGET)
debug : $(SHARED_DBG_TARGET)
endif

## If we have to build both the library & the test app
else ifeq ($(BUILD_TEST), yes)

ifeq ($(LIB_TYPE), static)
release : $(TARGET) $(TEST_TARGET)
debug : $(DBG_TARGET) $(TEST_DBG_TARGET)
else ifeq ($(LIB_TYPE), shared)
release : $(SHARED_TARGET) $(TEST_TARGET)
debug : $(SHARED_DBG_TARGET) $(TEST_DBG_TARGET)
endif

endif

##Make static libraries
$(TARGET) : $(OBJS) | $(LIB_DIR)
	@echo "Linking release build...."
	$(AR_FLAGS) $(R_FLAGS) $@ $^
	@echo "Linking release build completed"

$(DBG_TARGET) : $(DBG_OBJS) | $(LIB_DIR)
	@echo "Linking debug build...."
	$(AR_FLAGS) $(R_FLAGS) $@ $^
	@echo "Linking debug build completed"

##Make shared libraries
$(SHARED_TARGET) : $(OBJS) | $(LIB_DIR)
	@echo "Linking release build...."
	$(CXX) $(SHARED_FLAG) -o $@ $^
	@echo "Linking release build completed"

$(SHARED_DBG_TARGET) : $(DBG_OBJS) | $(LIB_DIR)
	@echo "Linking debug build...."
	$(CXX) $(SHARED_FLAG) -o $@ $^
	@echo "Linking debug build completed"

ifeq ($(LIB_TYPE), static)	##.a aka static lib making in progress...
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling release build...."
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiling release build completed"

$(OBJ_DIR)/%_d.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling debug build...."
	$(CXX) $(CXXFLAGS) -c $< -o $@ -DDEBUG
	@echo "Compiling debug build completed"

else ifeq ($(LIB_TYPE), shared)	##.so file making in progress...
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling release build...."
	$(CXX) $(CXXFLAGS) -c $(FPIC_FLAG) $< -o $@
	@echo "Compiling release build completed"

$(OBJ_DIR)/%_d.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling debug build...."
	$(CXX) $(CXXFLAGS) -c $(FPIC_FLAG) $< -o $@ -DDEBUG
	@echo "Compiling debug build completed"
endif

##Make tests with static lib
ifeq ($(LIB_TYPE), static)
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

## Make tests with shared lib
else ifeq ($(LIB_TYPE), shared)
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
endif

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
