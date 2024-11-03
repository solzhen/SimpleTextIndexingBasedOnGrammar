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