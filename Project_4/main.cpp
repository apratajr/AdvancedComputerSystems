//
// file: main.cpp
// desc: ACS Project 4 MAIN
// auth: Andrew Prata
//
// This program implements a dictionary codec which
// encodes raw column data into a compact form.
//

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

// Class for the encoding translation dictionary
class EncoderDictionary {
private:
    std::unordered_map<std::string, int> dictionary;
    int nextValue;
public:
    // Constructor
    EncoderDictionary() : nextValue(0) {}

    // Function to add a key
    int addKey(const std::string& key) {
        // Iterator to the KVP
        auto it = dictionary.find(key);
        // Key already exists, return its value
        if (it != dictionary.end()) {
            return it->second;
        // Key doesn't exist, add it with the next integer encoding
        } else {
            dictionary[key] = nextValue;
            return nextValue++;
        }
    }
    // Wrapper function for size() of internal dict
    size_t size() const {
        return dictionary.size();
    }
};

// Form the dictionary itself, with keys as the original data and values as the encodings
void createDictionary(const std::string& filepath_in, const std::string& dictpath_out, EncoderDictionary& d) {
    std::ofstream dict_out(dictpath_out);
    if (!dict_out.is_open()) {
        std::cerr << "Error: Could not open the dictionary file " << dictpath_out << '\n';
        return;
    }
    std::ifstream file_in(filepath_in);
    if (!file_in.is_open()) {
        std::cerr << "Error: Could not open the input file " << filepath_in << '\n';
        return;
    }
    std::string line;
    dict_out << std::hex;
    while (std::getline(file_in, line)) {
        int encoding = d.addKey(line);
        dict_out << line << ":" << encoding << '\n';
    }
    file_in.close();
    dict_out.close();
    std::cout << "DICTIONARY CREATED." << '\n';
    return;
}

// Scan input file, perform encoding against existing dictionary, write to file
void encode(const std::string& filepath_in, const std::string& filepath_out, EncoderDictionary& d) {
    std::ofstream file_out(filepath_out);
    if (!file_out.is_open()) {
        std::cerr << "Error: Could not open the output file " << filepath_out << '\n';
        return;
    }
    std::ifstream file_in(filepath_in);
    if (!file_in.is_open()) {
        std::cerr << "Error: Could not open the input file " << filepath_in << '\n';
        return;
    }
    std::string line;
    while (std::getline(file_in, line)) {
        int encoding = d.addKey(line);
        file_out << encoding << '\n';
    }
    file_out.flush();
}

int main(int argc, char* argv[]) {
    if (argc < 4) { // Ensure correct commandline arguments
        std::cerr << "Usage: " << argv[0] << " <path/to/input.txt>"
            " <path/to/output.txt> <path/to/dictionary.txt>" << '\n';
        return 1;   // Return an error code
    }
    std::string filepath_in = argv[1];
    std::string filepath_out = argv[2];
    std::string dictpath_out = argv[3];
    EncoderDictionary dictionary;

    createDictionary(filepath_in, dictpath_out, dictionary);
    encode(filepath_in, filepath_out, dictionary);

    std::cout.flush();
    return 0;
}