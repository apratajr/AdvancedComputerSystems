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
    int getEncoding(const std::string& key) {
        std::lock_guard<std::mutex> lock(dictionaryMutex);
        auto it = dictionary.find(key);
        if (it == dictionary.end()) {
            return -1;
        }
        return dictionary.find(key)->second;
    }

    std::vector<int> getEncodingValuesWithPrefix(const std::string& prefix) {
        std::lock_guard<std::mutex> lock(dictionaryMutex);
        auto lower = dictionary.lower_bound(prefix);
        auto upper = dictionary.upper_bound(prefix + char(255));
        std::vector<int> encodingValues;
        for (auto it = lower; it != upper; ++it) {
            encodingValues.push_back(it->second);
        }
        return encodingValues;
    }
};
```

This stucture is simple to understand, and is effectively a wrapper for the standard C++ `std::map` structure. This wrapper encorporates automatic encoding generation, and mutexing. These functions are critical for the functioning of the system. The `mutex` is of course required for multithreaded construction of the encoding dictionary. The `nextValue` private member is used to set the next encoding in the sequence when we encounter a new string at encoding time.


### Query
Given a query term (`target` string), the program provides the function of locating the index locations of the term in the encoded file.

Within this section, there are references to the *memory representation* of the input data, encoded data, and dictionary. The actual `.txt` files containing them are read at runtime, and these are simply vectorized versions of those files for use in memory.

Declarations in `Project_4\main.cpp` for in-memory testing data:

```cpp
//-- INDICES REPRESENT ROW OF COLUMN --//
std::vector<std::string> inputraw;
std::vector<int> inputencoded;

//-- DICTIONARY INSTANCE AS DESCRIBED IN PREVIOUS SECTION --//
EncoderDictionary dictionary;
```

#### Vanilla Implementation
The system allows for basic vanilla operations on the input file. Functions exist for reading the input file into memory and querying it (using a `target`) from its memory representation.

"Vanilla search" from the Project Handout was considered to be a standard **O(n)** scan through memory in search of the `target`. Vector pushbacks were found to have a negligible computational impact on system performance, but for consistency they remained identical for all implementations. See below the vanilla standard (VANSTD) search:

```cpp
//-- METHOD VANSTD --//
bool searchInput(const std::vector<std::string>& input_data, std::string target, std::vector<int>& target_locations) {
    for (size_t i = 0; i < input_data.size(); ++i) {
        if (input_data[i] == target) {
            target_locations.push_back(i);
        }
    }
    return (target_locations.size() != 0) ? true : false;
}
```
This of course bypasses all dictionary and encoding related optimizations. We simply perform a walk through the dataset (in our case, `inputraw`) as it is in memory, and compare each row to the `target`.

#### Encoded Implementation
The encoded functionality of the system is obviously of primary interest in this experiment, and its implementation (ENCSTD) is displayed below. Note that comments have been omitted here for brevity, but the full code with comments can be reviewed at `Project_4\EncoderDictionary.h` if interested.

```cpp
//-- METHOD ENCSTD --//
bool searchEncoded(const std::vector<int>& encoded_data, EncoderDictionary& d, std::string target, std::vector<int>& target_locations) {
    int target_encoding = d.getEncoding(target);
    if (target_encoding == -1) {
        return false;
    }
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        if (encoded_data[i] == target_encoding) {
            target_locations.push_back(i);
        }
    }
    return true;
}
```
In this implementation, we of course assume that the encoded file is preloaded in memory (as we assumed that the input was for VANSTD). Our first step is to acquire the encoding of our `target`. This involves simply querying the `EncoderDictionary` instance for the `target` encoding using the member function `getEncoding()`. This function, as you might guess from the code, either returns an unsigned integer encoding value, or `-1` if the `target` does not exist in the dictionary.

With our encoding, we then linearly search the encoded data vector in the same manner as VANSTD.

#### Encoded Implementation with SIMD Support
This is perhaps the most interesting implementation in the experiment, as it combines the efficiency of integer encoding with SIMD instructions, to perform a (theoretically) very efficient query search of the data. The method's implementation (ENCAVX) is shown below.

```cpp
//-- METHOD ENCAVX --//
bool searchEncodedSIMD(const std::vector<int>& encoded_data, EncoderDictionary& d, const std::string& target, std::vector<int>& target_locations) {
    int target_encoding = d.getEncoding(target);
    if (target_encoding == -1) {
        return false;
    }
    __m128i target_vec = _mm_set1_epi32(target_encoding);
    for (size_t i = 0; i < encoded_data.size(); i += 4) {
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&encoded_data[i]));
        __m128i result = _mm_cmpeq_epi32(data_vec, target_vec);
        int comparison_result[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(comparison_result), result);
        for (int j = 0; j < 4; ++j) {
            if (comparison_result[j] != 0) {
                target_locations.push_back(i+j);
            }
        }
    }
    return true;
}
```
To begin, we of course determine the encoding of the `target` using the same method as before with ENCSTD. Importantly, we then assume that the target encoding of any given `target:encoding` pair is less than the 32-bit integer limit. This allows us to use a 128-bit SIMD vector `target_vec` to load four copies of the target encoding. We will then loop over the encoded data vector in steps of 4, loading 128-bit SIMD vectors `data_vec` as we go. We then simply compare `data_vec` against `target_vec`, and record matches.

This division of the data source into SIMD-vector sized pieces is similar to previous experiments from this course.

### Prefix Scanning
Next, we will detail the prefix-scanning implementations for the three optimization levels already covered: vanilla (raw input - VANSTD), encoded standard (ENCSTD), and encoded with SIMD acceleration (ENCAVX).

"Prefix scanning" in this context means determining if and where in the original input a term starting with some specific string exists. Because the implementations of this task are effectively logical extensions of their previous Query Search counterparts, less detail will be provided to prevent repetition.

#### Vanilla Implementation
```cpp
//-- METHOD VANSTD --//
bool prefixSearchInput(const std::vector<std::string>& input_data, std::string target_prefix, std::vector<int>& target_locations) {
    for (size_t i = 0; i < input_data.size(); ++i) {
        if (input_data[i].compare(0, target_prefix.length(), target_prefix) == 0) {
            target_locations.push_back(i);
        }
    }
    return (target_locations.size() != 0) ? true : false;
}
```
The only major change to the vanilla implementation is that we now perform a `std::string::compare` of the first `N` characters of each input data value against the `target_prefix`, where `N`=(length of prefix). We then pushback each location where the comparison is valid. This is a trivial solution, and is relatively uninteresting.

#### Encoded Implementation
```cpp
//-- METHOD ENCSTD --//
bool prefixSearchEncoded(const std::vector<int>& encoded_data, EncoderDictionary& d, std::string target_prefix, std::vector<int>& target_locations) {
    std::vector<int> target_encodings = d.getEncodingValuesWithPrefix(target_prefix);
    if (target_encodings.size() == 0) {
        return false;
    }
    for (size_t i = 0; i < encoded_data.size(); ++i) {
        for (size_t j = 0; j < target_encodings.size(); ++j) {
            if (encoded_data[i] == target_encodings[j]) {
                target_locations.push_back(i);
            }
        }
    }
    return true;
}
```
With encodings, we cannot perform the same string comparison, as we only have integers. However, we can do something else which is theoretically even better.

We start by querying the dictionary for encoding values using the the `EncoderDictionary::getEncodingValuesWithPrefix` member function. This function is interesting compared to `getEncoding`, and strongly leverages the ordered nature of the data structure (it is covered in detail at the beginning of **Programming Deliverables**).

With a vector of encodings where our `target_prefix` condition is met, we then simply perform a nested loop check using this vector and the encoded data vector.

#### Encoded Implementation with SIMD Support
The code for the SIMD implementation of encoded prefix scanning is extremely similar to the SIMD Query Search, except that it has an nested loop that checks each of the prefix matches, in the same manner as the regular encoding prefix scanning implementation. Understanding these two prior functions is equivalent to understanding this one. For this reason, I will spare you another 35 line block of code... although it is available at `Project_4\EncoderDictionary.h`.

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