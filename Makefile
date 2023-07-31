include ./Make.helper

CXX_FLAGS=$(MY_CXX_FLAGS) $(MY_CXX_OPT_FLAGS) -I$(INC_DIR) -L$(LIB_DIR) 
CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 

SRC_DIR := src
BUILD_DIR := build

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
EXECS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.x, $(SRC_FILES))

all: $(EXECS)

build-test: $(EXECS)
	        
$(BUILD_DIR)/%.x: $(SRC_DIR)/%.cpp 
	$(MY_CXX) $(CXX_FLAGS) -o $@ $< $(CCLIB) 

clean:
	rm -f $(EXECS)
	rm -rf *.dSYM
