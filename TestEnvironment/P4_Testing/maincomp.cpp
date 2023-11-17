#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <zlib.h>

void compressFile(const std::string &inputFileName, const std::string &outputFileName) {
    std::ifstream inputFile(inputFileName, std::ios::binary);
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string inputString = buffer.str();

    const char* inputBuffer = inputString.c_str();
    uLong inputSize = static_cast<uLong>(inputString.size());

    // Allocate memory for compressed data
    uLong outputSize = compressBound(inputSize);
    std::vector<char> outputBuffer(outputSize);

    // Compress the data
    if (compress(reinterpret_cast<Bytef*>(&outputBuffer[0]), &outputSize, reinterpret_cast<const Bytef*>(inputBuffer), inputSize) != Z_OK) {
        std::cerr << "Compression failed!" << std::endl;
        return;
    }

    // Write compressed data to the output file
    outputFile.write(&outputBuffer[0], outputSize);

    inputFile.close();
    outputFile.close();

    std::cout << "Compression successful. Compressed size: " << outputSize << " bytes." << std::endl;
}

void decompressFile(const std::string &inputFileName, const std::string &outputFileName) {
    std::ifstream inputFile(inputFileName, std::ios::binary);
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return;
    }

    // Read the compressed data from the input file
    inputFile.seekg(0, std::ios::end);
    size_t inputSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::vector<char> inputBuffer(inputSize);
    inputFile.read(&inputBuffer[0], inputSize);

    // Allocate memory for decompressed data
    const size_t CHUNK_SIZE = 4096; // Adjust this size based on your needs
    std::vector<char> outputBuffer(CHUNK_SIZE);

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(&inputBuffer[0]);
    stream.avail_in = static_cast<uInt>(inputSize);
    stream.next_out = reinterpret_cast<Bytef*>(&outputBuffer[0]);
    stream.avail_out = static_cast<uInt>(CHUNK_SIZE);

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Decompression initialization failed!" << std::endl;
        return;
    }

    int ret;
    do {
        ret = inflate(&stream, Z_NO_FLUSH);
        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                std::cerr << "Decompression failed: " << zError(ret) << std::endl;
                inflateEnd(&stream);
                return;
        }

        // Write decompressed data to the output file
        size_t decompressedSize = CHUNK_SIZE - stream.avail_out;
        outputFile.write(&outputBuffer[0], decompressedSize);

        // Reset the output buffer
        stream.next_out = reinterpret_cast<Bytef*>(&outputBuffer[0]);
        stream.avail_out = static_cast<uInt>(CHUNK_SIZE);
    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);

    inputFile.close();
    outputFile.close();

    std::cout << "Decompression successful." << std::endl;
}


int main() {
    std::string inputFileName = "ColumnEnc.txt";
    std::string compressedFileName = "ColumnEncComp.z";
    std::string decompressedFileName = "ColumnEncDecomp.txt";

    // Compress the file
    compressFile(inputFileName, compressedFileName);

    // Decompress the file
    decompressFile(compressedFileName, decompressedFileName);

    return 0;
}