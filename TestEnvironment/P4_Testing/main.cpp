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
};

int countLines(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filepath << std::endl;
        return 0;
    }

    int totalLines = std::count(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
        '\n'
    ) + 1;
    return totalLines;
}

int main() {
    EncoderDictionary dict;

    std::cout << "K: 'apple', V: " << dict.addKey("apple") << std::endl;
    std::cout << "K: 'banana', V: " << dict.addKey("banana") << std::endl;
    std::cout << "K: 'apple', V: " << dict.addKey("apple") << std::endl;
    std::cout << "K: 'cherry', V: " << dict.addKey("cherry") << std::endl;

    /*
    auto start = std::chrono::high_resolution_clock::now();

    readAndEncode(filepath_, dictionary);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << "Time elapsed: " << duration.count() << " seconds." << std::endl;
    */

    /*
    while (std::getline(file_in, line)) {
        int encoding = d.addKey(line);
        file_out << "Input: " << std::setw(20) << std::left << line << "Encoding: " << encoding << std::endl;
    }
    */

    return 0;
}