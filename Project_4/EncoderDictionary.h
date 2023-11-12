//
// file: EncoderDictionary.h
// desc: ACS Project 4 Class Header
// auth: Andrew Prata
//
// This program implements a dictionary codec class,
// and its helper functions.
//

#include "FStreamHelper.h"
#include <map>
#include <string>
#include <mutex>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <thread>
#include <emmintrin.h>

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

    // Function to return a vector of encoding values for all keys that have a particular prefix
    std::vector<int> getEncodingValuesWithPrefix(const std::string& prefix) {
        std::lock_guard<std::mutex> lock(dictionaryMutex);

        auto lower = dictionary.lower_bound(prefix);
        auto upper = dictionary.upper_bound(prefix + char(255)); // char(255) is used to get the upper bound

        std::vector<int> encodingValues;
        for (auto it = lower; it != upper; ++it) {
            encodingValues.push_back(it->second);
            //std::cout << it->first << std::endl;
        }

        return encodingValues;
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
    std::ifstream file_in_async = openFileForReading(filepath_in);

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
    std::ofstream dict_out = openFileForWriting(dictpath_out);
    std::ifstream file_in = openFileForReading(filepath_in);

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
        size_t end = (i == num_threads-1) ? line_count : start + lines_per_thread;
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
    std::ofstream file_out = openFileForWriting(filepath_out);
    std::ifstream file_in = openFileForReading(filepath_in);

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
    std::ifstream dict_in = openFileForReading(dictpath_in);
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
    std::ifstream file_in = openFileForReading(filepath_in);
    std::string line;
    while (std::getline(file_in, line)) {
        input_data.push_back(line);
    }
    file_in.close();
}

// Functon to read an encoded file and throw it in DRAM
void readEncodedFile(const std::string& filepath_in_enc, std::vector<int>& encoded_data) {
    std::ifstream file_in_enc = openFileForReading(filepath_in_enc);
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

// Function to perform a vanilla PREFIX search on raw input in memory
void prefixSearchInput(const std::vector<std::string>& input_data, std::string target_prefix) {
    for (size_t i = 0; i < input_data.size(); ++i) {
        // Check to see if the prefix is present for the currently examined column index
        if (input_data[i].compare(0, target_prefix.length(), target_prefix) == 0) {
            std::cout << "VANILLA String prefix " << target_prefix << " match found at location: " << i << std::endl;
        }
    }
}

// Standard naive search of encoded column
void searchEncoded(const std::vector<int>& encoded_data, EncoderDictionary& d, std::string target) {
    // Get the encoding for the target string
    int target_encoding = d.getEncoding(target);
    // Loop over encoded data vector (integers)
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        // Indivudually check each element to see if it has the encoding for the target
        if (encoded_data[i] == target_encoding) {
            // If it does, output that we have found a match
            std::cout << "ENCSTD String " << target << " match found at location: " << i << std::endl;
        }
    }
}

// Standard naive PREFIX search of encoded column
void prefixSearchEncoded(const std::vector<int>& encoded_data, EncoderDictionary& d, std::string target_prefix) {
    // Get the encodings for the targets who match the prefix condition
    std::vector<int> target_encodings = d.getEncodingValuesWithPrefix(target_prefix);
    // Loop over encoded data vector (integers)
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        // Loop over the target encodings vector
        for (size_t j = 0; j < target_encodings.size(); ++j) {
            // Match found?
            if (encoded_data[i] == target_encodings[j]) {
                std::cout << "ENCSTD String prefix " << target_prefix << " match found at location: " << i << std::endl;
            }
        }
    }
}

// SIMD-accelerated search of encoded column
void searchEncodedSIMD(const std::vector<int>& encoded_data, EncoderDictionary& d, const std::string& target) {
    // Get the encoding for the target string
    int target_encoding = d.getEncoding(target);

    // Convert the target encoding to a vector for SIMD comparison
    __m128i target_vec = _mm_set1_epi32(target_encoding);

    // Loop over encodings with SIMD this time
    for (size_t i = 0; i < encoded_data.size(); i += 4) {
        // Load 4 integers from encoded_data into a SIMD register
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&encoded_data[i]));

        // Compare each element in the SIMD register with the target encoding
        __m128i result = _mm_cmpeq_epi32(data_vec, target_vec);

        // Store the comparison result in an array
        int comparison_result[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(comparison_result), result);

        // Check each comparison result
        for (int j = 0; j < 4; ++j) {
            if (comparison_result[j] != 0) {
                // If a match is found, output the location
                std::cout << "ENCAVX String " << target << " match found at location: " << i + j << std::endl;
            }
        }
    }
}

// SIMD-accelerated PREFIX search of encoded column
void prefixSearchEncodedSIMD(const std::vector<int>& encoded_data, EncoderDictionary& d, const std::string& target_prefix) {
    // Get the encodings for the targets who match the prefix condition
    std::vector<int> target_encodings = d.getEncodingValuesWithPrefix(target_prefix);

    // Form a std::vector of target AVX2 vectors for SIMD comparison
    std::vector<__m128i> target_vecs;
    for (size_t i = 0; i < target_encodings.size(); ++i) {
        target_vecs.push_back(_mm_set1_epi32(target_encodings[i]));
    }

    // Loop over encodings with SIMD this time
    for (size_t i = 0; i < encoded_data.size(); i += 4) {
        // Load 4 integers from encoded_data into a SIMD register
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&encoded_data[i]));

        for (size_t j = 0; j < target_vecs.size(); ++j) {
            // Compare each element in the SIMD register with the target encoding
            __m128i result = _mm_cmpeq_epi32(data_vec, target_vecs[j]);

            // Store the comparison result in an array
            int comparison_result[4];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(comparison_result), result);

            // Check each comparison result
            for (int k = 0; k < 4; ++k) {
                if (comparison_result[k] != 0) {
                    // If a match is found, output the location
                    std::cout << "ENCAVX String " << target_prefix << " match found at location: " << i + k << std::endl;
                }
            }
        }
    }
}