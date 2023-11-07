//
// file: main.cpp
// desc: ACS Project 4 MAIN
// auth: Andrew Prata
//
// This program performs matrix-matrix multiplication.
// It includes three optimization options for this
// operation. These are: multithreading, SIMD, and
// cache usage optimization.
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

void readAndEncode(const std::string& filepath, EncoderDictionary& d) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filepath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        int encoding = d.addKey(line);
        std::cout << "Input: " << std::setw(20) << std::left << line << "Encoding: " << encoding << std::endl;
    }
    file.close();
}

int main(void) {
    std::string filepath_ = "ColumnSmall.txt";
    EncoderDictionary dictionary;

    auto start = std::chrono::high_resolution_clock::now();

    readAndEncode(filepath_, dictionary);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Time elapsed: " << duration.count() << " seconds." << std::endl;

    return 0;
}