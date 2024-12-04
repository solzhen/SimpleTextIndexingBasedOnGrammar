#include <sdsl/int_vector.hpp>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

#include "globals.h"

std::string g_fileName;

int main(int argc, char* argv[]) {
    Catch::Session session; // There must be exactly one instance

    // Define a file name variable
    using namespace Catch::Clara;
    auto cli
        = session.cli()                // Get Catch2's default CLI
        | Opt(g_fileName, "file")      // Bind the global variable to this option
            ["-f"]["--file"]           // Option names
            ("Input file name");       // Description for help

    // Pass the new CLI back to Catch2
    session.cli(cli);

    // Parse the command-line arguments
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0) {
        return returnCode; // Return on command-line error
    }

    // Check if a file name was provided
    if (g_fileName.empty()) {
        g_fileName = "default_input.txt";
        std::cerr << "Error: No file name provided. Use --file <file_name>\n";
    }

    std::cout << "Using file: " << g_fileName << "\n";

    // Run the Catch2 tests
    return session.run();
}


TEST_CASE("sdsl::int_vector basic operations", "[int_vector]") {
    sdsl::int_vector<1> vec(10); // Create a 10-element vector
    vec[0] = 1;
    vec[1] = 0;
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 0);
    REQUIRE(vec.size() == 10);
}