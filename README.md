Requirements:

SDSL

https://github.com/simongog/sdsl-lite
Catch2

https://github.com/catchorg/Catch2

Download:

git clone https://github.com/solzhen/trabajoadelantado.git
cd trabajoadelantado


Compile with:

cmake -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build

Run:

./build/program.x -f filename

Tests

./build/tests.x [test_type] --file filename
