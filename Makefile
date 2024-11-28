include ./Make.helper

CXX_FLAGS=$(MY_CXX_FLAGS) $(MY_CXX_OPT_FLAGS) -I$(INC_DIR) -L$(LIB_DIR) -I$(BOOST_INC)
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64
REPAIR_DIR = repairs/repair110811/
LDFLAGS = $(REPAIR_DIR)/repair.o $(REPAIR_DIR)/encoder.o $(REPAIR_DIR)/bits.o

SRC_DIR := src
BUILD_DIR := build

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

EXECS := $(BUILD_DIR)/program.x

all: $(EXECS)

$(EXECS): $(OBJ_FILES)
	$(MY_CXX) -g $(CXX_FLAGS) -o $@ $^ $(CCLIB) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(MY_CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(EXECS) $(OBJ_FILES)
	rm -rf *.dSYM

TEST_DIR := tests
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

CATCH2_INCLUDE_DIR := /usr/local/include
CATCH2_LIB_DIR := /usr/local/lib

test:
	$(MY_CXX) $(CXX_FLAGS) -I$(CATCH2_INCLUDE_DIR) -L$(CATCH2_LIB_DIR) -o $(BUILD_DIR)/test.x $(wildcard $(TEST_FILES)/*.cpp) $(CCLIB) -lcatch2 $(LDFLAGS)