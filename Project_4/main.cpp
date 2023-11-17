//
// file: main.cpp
// desc: ACS Project 4 MAIN
// auth: Andrew Prata
//
// This program implements a dictionary codec which
// encodes raw column data into a convenient form.
//

#include "EncoderDictionary.h" // Core data structure and helpers
#include <chrono>              // Timing tasks

int main(int argc, char* argv[]) {
    if (argc < 5) { // Ensure correct commandline arguments
        std::cerr << "Usage: " << argv[0] << " <path/to/input.txt>"
            " <path/to/output.txt> <path/to/dictionary.txt>"
            " <regenerate dict/encfile? [1/0]>" << '\n';
        return 1;   // Return an error code
    }
    std::string filepath_in = argv[1];
    std::string filepath_out = argv[2];
    std::string dictpath = argv[3];
    int no_dict = atoi(argv[4]);

    EncoderDictionary dictionary; // The dictionary itself - how we translate between input and encoded
    std::vector<std::string> inputraw; // The input as it is in its txt
    std::vector<int> inputencoded; // The encoded input as it is in its txt

    if (no_dict) { // If the command line option to create a dictionary and encfile was selected
        createDictionary(filepath_in, dictpath, dictionary);
        createEncodedFile(filepath_in, filepath_out, dictionary);
    }

    std::cout << "\nLoading files." << "\n";
    auto startl = std::chrono::high_resolution_clock::now();
    // Assumption that dictpath, filepath_in, and filepath_out are all populated
    readDictionary(dictpath, dictionary);
    readInputFile(filepath_in, inputraw);
    readEncodedFile(filepath_out, inputencoded);
    auto stopl = std::chrono::high_resolution_clock::now();
    auto durationl = std::chrono::duration_cast<std::chrono::seconds>(stopl - startl);
    std::cout << "Files loaded. Time elapsed: " << durationl.count() << " sec.\n" << std::endl;

    // TESTING PARAMETERS
    // const std::string searchterm = "wzulz";
    // const std::string searchterm = "nsmgpo";
    // const std::string searchterm = "test";
    const std::string prefix = "ap";

    // -- TESTING TARGET SEARCH --

    // TESTING VANILLA STANDARD TARGET SEARCH
    // std::vector<int> hits;
    // std::cout << "\nSearching for target " << searchterm << " using method VANSTD" << "\n";
    // auto start = std::chrono::high_resolution_clock::now();
    // if (!searchInput(inputraw, searchterm, hits)) {
    //     std::cout << "Search target " << searchterm << " does not exist in the dataset." << "\n";
    //     exit(0);
    // }
    // auto stop = std::chrono::high_resolution_clock::now();
    // std::cout << "Search target " << searchterm << " found at location(s): ";
    // // for (size_t i = 0; i < hits.size(); ++i) {
    // //     std::cout << hits[i] << " ";
    // // }
    // std::cout << "\n";
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "VANSTD Time elapsed: " << duration.count() << " usec.\n" << std::endl;


    // // TESTING ENCODED STANDARD TARGET SEARCH
    // hits.clear();
    // std::cout << "Searching for target " << searchterm << " using method ENCSTD" << "\n";
    // start = std::chrono::high_resolution_clock::now();
    // if (!searchEncoded(inputencoded, dictionary, searchterm, hits)) {
    //     std::cout << "Search target " << searchterm << " does not exist in the dataset." << "\n";
    //     exit(0);
    // }
    // stop = std::chrono::high_resolution_clock::now();
    // std::cout << "Search target " << searchterm << " found at location(s): ";
    // // for (size_t i = 0; i < hits.size(); ++i) {
    // //     std::cout << hits[i] << " ";
    // // }
    // std::cout << "\n";
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "ENCSTD Time elapsed: " << duration.count() << " usec.\n" << std::endl;


    // // TESTING ENCODED SIMD TARGET SEARCH
    // hits.clear();
    // std::cout << "Searching for target " << searchterm << " using method ENCAVX" << "\n";
    // start = std::chrono::high_resolution_clock::now();
    // if (!searchEncodedSIMD(inputencoded, dictionary, searchterm, hits)) {
    //     std::cout << "Search target " << searchterm << " does not exist in the dataset." << "\n";
    //     exit(0);
    // }
    // stop = std::chrono::high_resolution_clock::now();
    // std::cout << "Search target " << searchterm << " found at location(s): ";
    // // for (size_t i = 0; i < hits.size(); ++i) {
    // //     std::cout << hits[i] << " ";
    // // }
    // std::cout << "\n";
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "ENCAVX Time elapsed: " << duration.count() << " usec.\n" << std::endl;

    // -- TESTING PREFIX SEARCH --

    // TESTING VANILLA STANDARD PREFIX SEARCH
    std::vector<int> hits;
    hits.clear();
    std::cout << "Searching for targets matching prefix " << prefix << " using method VANSTD" << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    if (!prefixSearchInput(inputraw, prefix, hits)) {
        std::cout << "Targets matching prefix " << prefix << " do not exist in the dataset." << "\n";
        exit(0);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "Targets matching prefix " << prefix << " found at location(s): ";
    for (size_t i = 0; i < hits.size(); ++i) {
        std::cout << hits[i] << " ";
    }
    std::cout << "\n";
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "VANSTD Time elapsed: " << duration.count() << " usec.\n" << std::endl;


    // TESTING ENCODED STANDARD PREFIX SEARCH
    hits.clear();
    std::cout << "Searching for targets matching prefix " << prefix << " using method ENCSTD" << "\n";
    start = std::chrono::high_resolution_clock::now();
    if (!prefixSearchEncoded(inputencoded, dictionary, prefix, hits)) {
        std::cout << "Targets matching prefix " << prefix << " do not exist in the dataset." << "\n";
        exit(0);
    }
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "Targets matching prefix " << prefix << " found at location(s): ";
    for (size_t i = 0; i < hits.size(); ++i) {
        std::cout << hits[i] << " ";
    }
    std::cout << "\n";
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "ENCSTD Time elapsed: " << duration.count() << " usec.\n" << std::endl;


    // TESTING ENCODED SIMD PREFIX SEARCH
    hits.clear();
    std::cout << "Searching for targets matching prefix " << prefix << " using method ENCAVX" << "\n";
    start = std::chrono::high_resolution_clock::now();
    if (!prefixSearchEncodedSIMD(inputencoded, dictionary, prefix, hits)) {
        std::cout << "Targets matching prefix " << prefix << " do not exist in the dataset." << "\n";
        exit(0);
    }
    stop = std::chrono::high_resolution_clock::now();
    std::cout << "Targets matching prefix " << prefix << " found at location(s): ";
    for (size_t i = 0; i < hits.size(); ++i) {
        std::cout << hits[i] << " ";
    }
    std::cout << "\n";
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "ENCAVX Time elapsed: " << duration.count() << " usec.\n" << std::endl;

    std::cout.flush();
    return 0;
}