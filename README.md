# SimpleTextIndexingBasedOnGrammar

Implementation of a Simplified Compressed Structure for Text Indexing Based on Grammars


---

## Requirements

Ensure the following dependencies are installed:

1. **[SDSL](https://github.com/simongog/sdsl-lite)**
   
   A library for succinct data structures.

2. **[Catch2](https://github.com/catchorg/Catch2)**
   
   A lightweight, header-only unit testing framework for C++.

---

## Download

Clone this repository to your local machine:

```bash
git clone https://github.com/solzhen/SimpleTextIndexingBasedOnGrammar.git
cd trabajoadelantado
```

---

## Clone external repository cxxopts
```bash
cd external
git clone https://github.com/jarro2783/cxxopts.git
```

---

## Compile

Compile the re-pair program:
```bash
cd repairs/repairnavarro
make
```

Build the project using CMake:

```bash
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Run

Execute the main program with the following command:

```bash
./build/program.x -f filename
```

Replace `filename` with the path to your input file.

---

## Tests

Run the test suite by specifying the test type and input file:

```bash
./build/tests.x [test_type] --file filename
```

- Replace `[test_type]` with the desired type of test to run.
- Replace `filename` with the path to your input file.

---

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). Feel free to use, modify, and distribute the code as you see fit.

---


