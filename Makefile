NAME = acme

CXX = g++
RM = rm -rf

SRC_DIR = src
TEST_DIR = tests
DIST_DIR = target

CFLAGS_COMMON = -std=gnu++20 -Wall -Wextra
LIB_CFLAGS = $(shell pkg-config --cflags --libs x11) -lstdc++

CFLAGS = $(CFLAGS_COMMON) -O2
TFLAGS = $(CFLAGS_COMMON) -O0 -g3 --coverage

# no need to change below this line
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)

ifndef DEBUG
MODE = release
else
MODE = debug
CFLAGS = $(TFLAGS)
endif

OBJ_DIR = $(DIST_DIR)/$(MODE)/obj
TEST_OBJ_DIR = $(DIST_DIR)/$(MODE)/obj/test

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))

LIB_OBJS = $(filter-out $(DIST_DIR)/$(MODE)/obj/main.o,$(OBJS))

TEST_TARGETS = $(patsubst $(TEST_DIR)/%.cpp,$(DIST_DIR)/%.test.$(MODE),$(TEST_SRCS))

default: $(DIST_DIR)/$(NAME).$(MODE)

RDESC_DIR := rdesc
RDESC_FEATURES := full

$(RDESC_DIR)/rdesc.mk:
	git clone https://github.com/metwse/rdesc.git $(RDESC_DIR) \
		--branch v0.2.0-rc.2

include $(RDESC_DIR)/rdesc.mk


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@ -MMD

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@ -MMD

$(DIST_DIR)/$(NAME).$(MODE): $(OBJS) $(RDESC) | $(DIST_DIR)
	$(CXX) $(CFLAGS) $^ -o $@ $(LIB_CFLAGS)

$(DIST_DIR)/%.test.$(MODE): $(TEST_OBJ_DIR)/%.o $(LIB_OBJS) $(RDESC) | $(DIST_DIR)
	$(CXX) $(CFLAGS) $^ -o $@ $(LIB_CFLAGS)

$(DIST_DIR) $(TEST_OBJ_DIR) $(OBJ_DIR):
	mkdir -p $@

tests: $(TEST_TARGETS)
	@echo > /dev/null

all: default tests

docs:
	doxygen

clean:
	$(RM) $(DIST_DIR) docs

.SECONDARY: $(OBJS) $(TEST_OBJS)
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

.PHONY: default tests all clean docs
