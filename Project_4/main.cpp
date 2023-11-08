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

void readAndEncode(const std::string& filepath_in, const std::string& filepath_out, EncoderDictionary& d) {
    std::ofstream file_out(filepath_out);
    if (!file_out.is_open()) {
        std::cerr << "Error: Could not open the output file " << filepath_out << std::endl;
        return;
    }
    std::ifstream file_in(filepath_in);
    if (!file_in.is_open()) {
        std::cerr << "Error: Could not open the input file " << filepath_in << std::endl;
        return;
    }
    std::string line;
    file_out << std::hex;
    while (std::getline(file_in, line)) {
        int encoding = d.addKey(line);
        file_out << encoding << std::endl;
    }
    file_in.close();
    file_out.close();
}

int main(void) {
    std::string filepath_in = "Column.txt";
    std::string filepath_out = "ColumnEncoded2.txt";
    EncoderDictionary dictionary;

    readAndEncode(filepath_in, filepath_out, dictionary);

    return 0;
}