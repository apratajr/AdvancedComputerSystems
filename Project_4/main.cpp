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
#include <map>
#include <iomanip>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>

// Class for the encoding translation dictionary
class EncoderDictionary {
private:
    std::map<std::string, int> dictionary;
    int nextValue;
    std::mutex dictionaryMutex; // Mutex for contruction synchronization
public:
    // Constructor
    EncoderDictionary() : nextValue(0) {}

    // Function to add a key
    int addKey(const std::string& key) {
        std::lock_guard<std::mutex> lock(dictionaryMutex); // Lock the mutex to prevent thread collision
        //std::cout << "Structure locked" << std::endl;
        // Iterator to the KVP
        auto it = dictionary.find(key);
        // Key already exists, return its value
        if (it != dictionary.end()) {
            //std::cout << "Exiting scope, releasing lock" << std::endl;
            return it->second;
        // Key doesn't exist, add it with the next integer encoding
        } else {
            dictionary[key] = nextValue;
            //std::cout << "Exiting scope, releasing lock" << std::endl;
            return nextValue++;
        }
    }

    // Function to retrieve a key encoding value
    int getEncoding(const std::string& key) {
        std::lock_guard<std::mutex> lock(dictionaryMutex); // Lock the mutex (this may be multithreaded eventually)
        return dictionary.find(key)->second;
    }

    // Function to provide access to the internal dictionary
    std::map<std::string, int>& getDictionary() {
        return dictionary;
    }

    // Wrapper function for size() of internal dict
    size_t size() const {
        return dictionary.size();
    }

    // Function to access the dictionary mutex (not needed just yet.)
    std::mutex& getMutex() {
        return dictionaryMutex;
    }
};

// Asynchronously process a portion of the input and form the correlated portion of dictionary d
void asyncProcessor(const std::string& filepath_in, size_t start, size_t end, EncoderDictionary& d) {
    // Open & Verify
    std::ifstream file_in_async(filepath_in);
    if (!file_in_async.is_open()) {
        std::cerr << "Error: Could not open the input file " << filepath_in << '\n';
        return;
    }

    // Seek to the beginning of the desired line
    for (size_t i = 0; i < start; ++i) {
        file_in_async.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Process lines from start to end of thread region
    std::string line;
    size_t current_position = start;
    while (current_position < end && std::getline(file_in_async, line)) {
        d.addKey(line); // Mutex critical region. Must be multithread protected (see class)
        current_position++;
    }
}

// Form the dictionary itself, with keys as the original data and values as the encodings
void createDictionary(const std::string& filepath_in, const std::string& dictpath_out, EncoderDictionary& d) {
    // Open and verify streams
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

    // Prepare for multithreading split
    const size_t num_threads = std::thread::hardware_concurrency(); // Get the number of available hardware threads
    size_t line_count = 0;
    std::string line;
    while (std::getline(file_in, line)) {
        ++line_count;
    }
    const size_t lines_per_thread = floor(line_count / num_threads);
    std::vector<std::thread> threads;

    // Divide work over thread count and asynchronously build dictionary data structure
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * lines_per_thread;
        size_t end = (i == num_threads-1) ? line_count-1 : start + lines_per_thread;
        threads.emplace_back([&, start, end](){ asyncProcessor(filepath_in, start, end, d); });
    }

    // Synchronize all threads
    for (size_t i = 0; i < num_threads; ++i) {
        threads[i].join();
    }

    // Contruct dictionary text file (singlethreaded, otherwise mutex issues)
    dict_out << std::hex; // Testing of encoded sizes and such
    for (const auto& entry : d.getDictionary()) {
        dict_out << entry.first << ":" << entry.second << '\n';
    }
    file_in.close();
    dict_out.flush();
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
        int encoding = d.getEncoding(line);
        file_out << encoding << '\n';
    }
    file_in.close();
    file_out.flush();
    file_out.close();
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