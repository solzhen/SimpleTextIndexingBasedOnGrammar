cmake_minimum_required(VERSION 3.12)
project(MEMORIA)

# Set the source files directory
set(SOURCE_DIR src)

# Set the program name
set(PROGRAM_NAME program.x)

# Set the repairs directory
set(REPAIRS_DIR repairs/repair110811)

# Add the source files to the project
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)

# Add the specific object files from the repairs directory
set(REPAIRS_OBJECTS
    ${REPAIRS_DIR}/repair.o
    ${REPAIRS_DIR}/encoder.o
    ${REPAIRS_DIR}/bits.o
)

# Manually specify the SDSL library and include directories
set(SDSL_LIB_DIR /home/cfnts/lib)
set(SDSL_INC_DIR /home/cfnts/include)

# Include the SDSL directories
include_directories(${SDSL_INC_DIR})
link_directories(${SDSL_LIB_DIR})

# Add the coroutine flag to the compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

# Add general flags (equivalent to MY_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -DNDEBUG -Wno-ignored-qualifiers -Wno-deprecated-copy -Wno-unused-parameter")

# Add optimization-specific flags (equivalent to MY_CXX_OPT_FLAGS)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops -msse4.2 -march=native")

# Compile the main program
add_executable(${PROGRAM_NAME} ${SOURCES} ${REPAIRS_OBJECTS})

# Link the SDSL library
target_link_libraries(${PROGRAM_NAME} sdsl divsufsort divsufsort64)

# TESTING
find_package(Catch2 3 REQUIRED)

# Add the test source files (separate from main program)
set(TEST_DIR test)
file(GLOB TEST_SOURCES ${TEST_DIR}/*.cpp)

# Add the test executable (link only the test sources)
add_executable(tests.x ${TEST_SOURCES} ${REPAIRS_OBJECTS} src/wavelet_matrix.cpp src/ppbv.cpp)

# Include the source directory for header files
target_include_directories(tests.x PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Link Catch2 and any necessary libraries to the test executable
target_link_libraries(tests.x PRIVATE Catch2::Catch2WithMain sdsl divsufsort divsufsort64)

# Set compiler flags for the test executable (if needed)
target_compile_options(tests.x PRIVATE -std=c++20 -Wall -Wextra)

# Register the tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(tests.x)
