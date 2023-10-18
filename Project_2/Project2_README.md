# Project 2: Matrix Multiplication Optimization

## Submission Information

**Andrew Prata, (https://github.com/apratajr/AdvancedComputerSystems/)**

*ECSE 4320: Advanced Computer Systems* Fall 2023

Due: 18-Oct-2023

## Installation and Usage

### Installing matTest
The name of the testing program is *Matrix Multiplication Optimization Testing* (`matTest.exe`). To install the program on your computer and perform testing yourself, you can either clone the entire repository, or download the C++ source files only. Assuming that the class header file is located in the same folder as the main file, the following `g++` command is valid from this folder:

    g++ ./main.cpp -o matTest.exe

This command will generate the `matTest.exe` executable file. This is technically all that you need for installation! See below for running the program. 
### Using matTest
Ensure that you are in the directory where `matTest` is located. The command-line syntax for running this program in your command window is as follows:

    matTest <multithreading> <simd> <cache optimization> <matrix type> <matrix size>

The arguments listed can be set as follows:

- `multithreading`: `0` for disabled, `1` for enabled
- `simd`: same as above
- `cache optimization`: same as above
- `matrix type`: `int` for fixed-point integer matrices, `float` for floating point number matrices
- `matrix size`: an integer to define the row and column dimensions of the matrices (matrices are square)

An example usage is shown below:

    matTest 1 0 0 float 1500

This exact syntax will perform matrix-matrix multiplication using two 1500 x  1500 matrices whose elements are of type float. It will perform this multiplication with Multithreading enabled, SIMD disabled, and Cache Optimization disabled.

### Sample Output for matTest

The output for the example at the end of the last subsection is shown below. Of primary interest to our upcoming analysis is the `Elapsed time` for matrix multiplication under different states of optimization (this is the last line of the command output).

    E:\Path\To\Program\Directory> matTest 1 0 0 float 1500

            Matrix Multiplication Optimization Testing
            Program Version 10.17.0
            __________________________________________

            Proceeding with settings
             multiThreading = true
             SIMD = false
             cacheOptimization = false
             matrix type = float
             matrix size = 1500 x 1500

            Generating random 1500 x 1500 matrices A and B ...

            Matrices populated. Elapsed time: 0.342729 seconds.

            Computing product A x B = C now ...

            Matrices multiplied. Elapsed time: 2.914116 seconds.

*Note: The computer that produced these results has a Core i7-12700K with 20 threads. Try this command with your CPU! If your CPU is better/worse than this benchmark, expect a lower/higher `Elapsed time`, respectively.*

## Experimental Results
This section will present experimental data for performance when multiplying matrices of different sizes and different data types.

***DATA/DISCUSSION HERE***

## Optimization Performance Comparison
This section will present data for all possible combinations of the implemented optimizations (`multithreading`, `simd`, and `cache optimization`).

For this testing, a constant matrix size of `1500 x 1500` was assumed, and the `int` data type was used. Thus, the only differences in the command run were to the `X`, `Y`, and `Z` fields below, which correspond to the optimization enable fields:

    matTest X Y Z int 1500

***DATA/DISCUSSION HERE***