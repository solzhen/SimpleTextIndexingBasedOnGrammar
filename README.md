Requirements:

SDSL
https://github.com/simongog/sdsl-lite
Catch2
https://github.com/catchorg/Catch2

Compile with:
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build

Run:
./build/program.x -f filename

Tests
./build/tests.x [test_type] --file filename
