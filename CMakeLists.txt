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

# Add cxxopts
add_subdirectory(external/cxxopts)

# Add optimization-specific flags (equivalent to MY_CXX_OPT_FLAGS)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -funroll-loops -msse4.2 -march=native -DNDEBUG")

# Configuración común (aplicable a todos los modos)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines -std=c++20 -Wno-ignored-qualifiers -Wno-deprecated-copy -Wno-unused-parameter")

# Configuración de Debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")

# Compile the main program
add_executable(${PROGRAM_NAME} ${SOURCES} ${REPAIRS_OBJECTS})

# Link the SDSL library
target_link_libraries(${PROGRAM_NAME} sdsl divsufsort divsufsort64 cxxopts)

# TESTING
find_package(Catch2 3 REQUIRED)

# Add the test source files
set(TEST_DIR test)
file(GLOB TEST_SOURCES ${TEST_DIR}/*.cpp)

# Add the test executable
set(IMPLEMENTATION_SOURCES
    src/wavelet_matrix.cpp
    src/ppbv.cpp
    src/sequences.cpp
    src/permutations.cpp
    src/grid.cpp
    src/file_handler.cpp
    src/patterns.cpp
    src/debug_config.cpp
)

add_executable(tests.x ${TEST_SOURCES} ${REPAIRS_OBJECTS} ${IMPLEMENTATION_SOURCES})

# Include the source directory for header files
target_include_directories(tests.x PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Link Catch2 and any necessary libraries to the test executable
#target_link_libraries(tests.x PRIVATE Catch2::Catch2WithMain sdsl divsufsort divsufsort64)

target_link_libraries(tests.x PRIVATE Catch2::Catch2 sdsl divsufsort divsufsort64)


# Set compiler flags for the test executable
target_compile_options(tests.x PRIVATE -std=c++20 -Wall -Wextra)

# Register the tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(tests.x)
