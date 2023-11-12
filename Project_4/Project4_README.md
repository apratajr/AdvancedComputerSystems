# Project 4: Dictionary Codec Implementation

## Submission Information

**Andrew Prata, (https://github.com/apratajr/AdvancedComputerSystems/)**

*ECSE 4320: Advanced Computer Systems* Fall 2023

Due: 11-Nov-2023

## Programming Deliverables
This section will cover the implementation of the system, and its basic functionality. Many issues were encountered along the way, and these, along with an overview of the development process, will be detailed as well.

The system is implemented in C++. due to its relative speed compared with other languages that I have experience in.

### Encoding
Given a raw column file, the system provides the function of creating an encoded equivalent, and writing it to the SSD. In this process, a dictionary which contains key,value pairs is created in memory. This is also written to the SSD for later lookup/scanning.

The data structure for the dictionary is shown below (it has been slimmed down for simplicity of explanation):
```cpp
class EncoderDictionary {
private:
    std::map<std::string, int> dictionary;
    int nextValue;
    std::mutex dictionaryMutex;
public:
    EncoderDictionary() : nextValue(0) {}

    int addKey(const std::string& key) {
        std::lock_guard<std::mutex> lock(dictionaryMutex);
        auto it = dictionary.find(key);

        if (it != dictionary.end()) {
            return it->second;
        } else {
            dictionary[key] = nextValue;
            return nextValue++;
        }
    }
};
```

This stucture is simple to understand, and is effectively a wrapper for the standard C++ `std::map` structure. This wrapper encorporates automatic encoding generation, and mutexing. These functions are critical for the functioning of the system. The `mutex` is of course required for multithreaded construction of the encoding dictionary. The `nextValue` private member is used to set the next encoding in the sequence when we encounter a new string at encoding time.


### Query
Given a query term (`target` string), the program provides the function of locating the index locations of the term in the encoded file.

### Vanilla Implementation
The system also allows for basic vanilla operations on the input file. Functions exist for reading the input file into memory and querying it (using a `target`) from its memory representation.

The input column is stored in memory as:
```cpp
std::vector<std::string> inputraw;
```
where each of the indices correlate with a row in the input file.

"Vanilla search" from the Project Handout was considered to be a standard **O(n)** scan through memory in search of the `target`:

```cpp
void searchInput(const std::vector<std::string>& input_data, std::string target) {
    for (size_t i = 0; i < input_data.size(); ++i) {
        if (input_data[i] == target) {
            std::cout << "String " << target << " match found at location: ";
            std::cout << i << std::endl;
        }
    }
}
```
This of course bypasses all dictionary and encoding related optimizations.

## Analysis
### Subsec1
X

### Subsec2
X

## Sec3
X

### Subsec1
X
### Subsec2
X