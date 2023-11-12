//
// file: FStreamHelper.h
// desc: ACS Project 4 File Stream Helper
// auth: Andrew Prata
//
// This program implements basic file opening helpers.
//

#include <iostream>
#include <fstream>

// Helper function to open/verify a filestream for reading
std::ifstream openFileForReading(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filepath << '\n';
    }
    return file;
}

// Helper function to open/verify a filestream for writing
std::ofstream openFileForWriting(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << filepath << '\n';
    }
    return file;
}