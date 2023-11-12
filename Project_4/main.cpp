//
// file: main.cpp
// desc: ACS Project 4 MAIN
// auth: Andrew Prata
//
// This program implements a dictionary codec which
// encodes raw column data into a convenient form.
//

#include "EncoderDictionary.h"
#include <chrono>
#include <algorithm>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc < 4) { // Ensure correct commandline arguments
        std::cerr << "Usage: " << argv[0] << " <path/to/input.txt>"
            " <path/to/output.txt> <path/to/dictionary.txt>" << '\n';
        return 1;   // Return an error code
    }
    std::string filepath_in = argv[1];
    std::string filepath_out = argv[2];
    std::string dictpath = argv[3];

    EncoderDictionary dictionary; // The dictionary itself - how we translate between input and encoded
    std::vector<std::string> inputraw; // The input as it is in its txt
    std::vector<int> inputencoded; // The encoded input as it is in its txt

    //createDictionary(filepath_in, dictpath, dictionary);
    readDictionary(dictpath, dictionary);
    //createEncodedFile(filepath_in, filepath_out, dictionary);

    const std::string searchterm = "wzulz";
    //const std::string searchterm = "nsmgpo";

    readInputFile(filepath_in, inputraw);
    readEncodedFile(filepath_out, inputencoded);

    // auto start = std::chrono::high_resolution_clock::now();
    // searchInput(inputraw, searchterm);
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "VANSTD Time elapsed: " << duration.count() << " usec." << std::endl;

    // start = std::chrono::high_resolution_clock::now();
    // searchEncoded(inputencoded, dictionary, searchterm);
    // stop = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "ENCSTD Time elapsed: " << duration.count() << " usec." << std::endl;

    // start = std::chrono::high_resolution_clock::now();
    // searchEncodedSIMD(inputencoded, dictionary, searchterm);
    // stop = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "ENCAVX Time elapsed: " << duration.count() << " usec." << std::endl;

    std::vector<int> test = dictionary.getEncodingValuesWithPrefix("wz");


    std::cout.flush();
    return 0;
}