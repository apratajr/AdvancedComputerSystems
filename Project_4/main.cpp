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
#include <emmintrin.h> // SSE2 intrinsics

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
    const size_t num_threads = 1;//std::thread::hardware_concurrency(); // Get the number of available hardware threads
    size_t line_count = 0;
    std::string line;
    while (std::getline(file_in, line)) {
        ++line_count;
    }
    const size_t lines_per_thread = floor(line_count / num_threads);
    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();

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

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Encoding Complete. Time elapsed: " << duration.count() << " useconds." << std::endl;

    // Contruct dictionary text file (singlethreaded, otherwise mutex issues)
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
void createEncodedFile(const std::string& filepath_in, const std::string& filepath_out, EncoderDictionary& d) {
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

// Function to read in an existing dictionary file for use with an encoded file
void readDictionary(const std::string& dictpath_in, EncoderDictionary& d) {
    // Open and verify stream
    std::ifstream dict_in(dictpath_in);
    if (!dict_in.is_open()) {
        std::cerr << "Error: Could not open the dictionary file " << dictpath_in << '\n';
        return;
    }
    std::string line;
    while (std::getline(dict_in, line)) {
        size_t kvp_split = line.find(':');
        std::string key = line.substr(0, kvp_split);
        int val = std::stoi(line.substr(kvp_split+1));
        d.getDictionary()[key] = val;
    }
}

// Function to read a vanilla column file and throw it in DRAM
void readInputFile(const std::string& filepath_in, std::vector<std::string>& input_data) {
    std::ifstream file_in(filepath_in);
    if (!file_in.is_open()) {
        std::cerr << "Error: Could not open the input file " << filepath_in << '\n';
        return;
    }
    std::string line;
    while (std::getline(file_in, line)) {
        input_data.push_back(line);
    }
    file_in.close();
}

// Functon to read an encoded file and throw it in DRAM
void readEncodedFile(const std::string& filepath_in_enc, std::vector<int>& encoded_data) {
    std::ifstream file_in_enc(filepath_in_enc);
    if (!file_in_enc.is_open()) {
        std::cerr << "Error: Could not open the encoded file " << filepath_in_enc << '\n';
        return;
    }
    std::string line;
    while (std::getline(file_in_enc, line)) {
        encoded_data.push_back(std::stoi(line));
    }
    file_in_enc.close();
}

// Function to perform a vanilla search on raw input in memory
void searchInput(const std::vector<std::string>& input_data, std::string target) {
    for (size_t i = 0; i < input_data.size(); ++i) {
        if (input_data[i] == target) {
            std::cout << "VANILLA String " << target << " match found at location: " << i << std::endl;
        }
    }
}

// Standard naive approach to locating the desired encoding
void searchEncoded(const std::vector<int>& encoded_data, EncoderDictionary& d, std::string target) {
    // Get the encoding for the target string
    int targetEncoding = d.getEncoding(target);
    // Loop over encodings (integers)
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        // Indivudually check each element to see if it has the encoding for the target
        if (encoded_data[i] == targetEncoding) {
            // If it does, output that we have found a match
            std::cout << "ENCSTD String " << target << " match found at location: " << i << std::endl;
        }
    }
}

void searchEncodedSIMD(const std::vector<int>& encoded_data, EncoderDictionary& d, const std::string& target) {
    // Get the encoding for the target string
    int targetEncoding = d.getEncoding(target);

    // Convert the target encoding to a vector for SIMD comparison
    __m128i targetVec = _mm_set1_epi32(targetEncoding);

    // Loop over encodings with SIMD this time
    for (size_t i = 0; i < encoded_data.size(); i += 4) {
        // Load 4 integers from encoded_data into a SIMD register
        __m128i dataVec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&encoded_data[i]));

        // Compare each element in the SIMD register with the target encoding
        __m128i result = _mm_cmpeq_epi32(dataVec, targetVec);

        // Store the comparison result in an array
        int comparisonResult[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(comparisonResult), result);

        // Check each comparison result
        for (int j = 0; j < 4; ++j) {
            if (comparisonResult[j] != 0) {
                // If a match is found, output the location
                std::cout << "ENCAVX String " << target << " match found at location: " << i + j << std::endl;
            }
        }
    }
}

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

    //const std::string searchterm = "wzulz";
    const std::string searchterm = "nsmgpo";

    readInputFile(filepath_in, inputraw);
    readEncodedFile(filepath_out, inputencoded);

    auto start = std::chrono::high_resolution_clock::now();
    searchInput(inputraw, searchterm);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "VANSTD Time elapsed: " << duration.count() << " usec." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    searchEncoded(inputencoded, dictionary, searchterm);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "ENCSTD Time elapsed: " << duration.count() << " usec." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    searchEncodedSIMD(inputencoded, dictionary, searchterm);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "ENCAVX Time elapsed: " << duration.count() << " usec." << std::endl;

    std::cout.flush();
    return 0;
}