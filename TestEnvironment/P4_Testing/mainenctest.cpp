#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

// Encode an integer using Varint encoding
void encodeVarint(int value, std::string& output) {
    google::protobuf::io::StringOutputStream output_stream(&output);
    google::protobuf::io::CodedOutputStream coded_output(&output_stream);
    coded_output.WriteVarint32(value);
}

// Decode an integer from Varint encoded data
int decodeVarint(const std::string& input, size_t& offset) {
    google::protobuf::io::ArrayInputStream input_stream(input.data(), input.size());
    google::protobuf::io::CodedInputStream coded_input(&input_stream);
    uint32_t value;
    if (coded_input.ReadVarint32(&value)) {
        offset += coded_input.CurrentPosition();
        return static_cast<int>(value);
    }
    return 0; // Error handling
}

int main() {
    std::vector<int> originalIntegers = {1985, 1986, 1987, 1988, 1989};
    std::string encodedData;

    for (int value : originalIntegers) {
        encodeVarint(value, encodedData);
    }

    // Write the encoded data to a file
    std::ofstream file_out("EncodedData.bin", std::ios::binary);
    file_out.write(encodedData.c_str(), encodedData.size());
    file_out.close();

    // Read the encoded data and decode the integers
    std::ifstream file_in("EncodedData.bin", std::ios::binary);
    std::string encodedDataFromFile((std::istreambuf_iterator<char>(file_in)), std::istreambuf_iterator<char>());
    size_t offset = 0;
    std::vector<int> decodedIntegers;

    while (offset < encodedDataFromFile.size()) {
        int decodedValue = decodeVarint(encodedDataFromFile, offset);
        decodedIntegers.push_back(decodedValue);
    }

    // Display the decoded integers
    for (int value : decodedIntegers) {
        std::cout << "Decoded Value: " << value << std::endl;
    }

    return 0;
}