# Project 2: Matrix Multiplication Optimization

## Submission Information

**Andrew Prata, (https://github.com/apratajr/AdvancedComputerSystems/)**

*ECSE 4320: Advanced Computer Systems* Fall 2023

Due: 18-Oct-2023

## Installation, Usage, and Structure

### Installing matTest
The name of the testing program is *Matrix Multiplication Optimization Testing* (`matTest.exe`). To install the program on your computer and perform testing yourself, you can either clone the entire repository, or download the C++ source files only. Assuming that the class header file is located in the same folder as the main file, the following `g++` command is valid from this folder:

    g++ ./main.cpp -o matTest.exe -mavx2 -mfma

This command will generate the `matTest.exe` executable file. The compiler flags `-mavx2` and `-mfma` are necessary for compilation of the AVX intrinsics that are used in the SIMD implementation. This is technically all that you need for installation! See below for running the program. 
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

*Usage note: If using the SIMD argument, a multiple of `8` must be used as the `matrix size`. This is due to the underlying structure of the SIMD multiplication implementation.*

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

### matTest Structure Overview
The `main.c` file contains the testing code, where the user selects (via the commandline) which optimizations they would like active. These selections set global flags, which determine which of the many matrix multiplication functions should be used. Part of this process is shown below:

```cpp
    if (multiThreading && !SIMD && !cacheOptimization) {
        Matrix<T> result = mulMatMT(A, B);
    }
    else if (!multiThreading && SIMD && !cacheOptimization) {
        Matrix<T> result = mulMatSIMD(A, B);
    }
    else if (!multiThreading && !SIMD && cacheOptimization) {
        Matrix<T> result = mulMatCO(A, B);
    } 
    ...
    ...
```

The actual algorithm implementations are located in `Matrix.h`, as well as (of course), the actual Matrix class used for all of the experiments (`A` and `B` from above are instances of this class). The private members of the class are shown below, it is a very simple wrapper-style class:

```cpp
    // Matrix class for compact storing and accessing
    template <typename T>
    class Matrix {
    private:
        std::vector<std::vector<T>> data;
        size_t rows;
        size_t cols;
    ...
    ...
```

The class is templated, such that it may be populated with any data type that might be required.

## Experimental Results - Matrix Size and Type
This section will present experimental data for performance when multiplying matrices of different sizes using the maximum performance algorithm and the simplest possible algorithm.

### Maximum Efficiency Implementation

First, here is a look at the best algorithm in the program for matrix-matrix multiplication:

<div style="text-align: center;">
<img src="sizecomparison_maximum.png"></div>

The tabulated max performance data is available below for reference.

    Size    Time (s)
    104	    0.001524
    208	    0.004872
    416	    0.02531
    832	    0.088071
    1664	0.483897
    3328	3.576969
    6656	28.068972
    13312	236.760115

Note that the final two data points listed above were omitted from the graph, as their extremely high relative values caused undesirable compression of the Time-axis.

### Naive Implementation

Next, here is the performance with no optimizations active. This is sometimes referred to as the "naive" implementation due to its strict adherence to the actual mathematical process of multiplication...

<div style="text-align: center;">
<img src="sizecomparison_naive.png"></div>

The tabulated naive implementation data is available below for reference.

    Size    Time (s)
    104	    0.005697
    208	    0.043937
    416	    0.352448
    832	    3.019665
    1664	29.424538
    3328	311.60638
    6656	Incomplete
    13312	Incomplete


## Optimization Performance Comparison
This section will present data for all possible combinations of the implemented optimizations (`multithreading`, `simd`, and `cache optimization`).

For this testing, a constant matrix size of `1664 x 1664` was assumed, and the `int` data type was used. Thus, the only differences in the command run were to the `X`, `Y`, and `Z` fields below, which correspond to the optimization enable fields:

    matTest X Y Z int 1664

### Collected Data Accross All Algorithms
Here are the results accross all possible configurations of the implemented optimizations:

<div style="text-align: center;">
<img src="algocomparison_1664.png"></div>

Note that unlike a typical bar graph, *lower* y-values are actually better here. The y-axis represents the time required to compute the product of two `1664 x 1664` integer matrices.

## Experiment Analysis and Conclusion

As shown in the previous sections, two main topics were studied for the three optimization techniques. The first topic was the scaling of performance with different matrix element data types, and different matrix sizes. The second topic was the performance of each optimization, and combinations of all three, with respect to the naive implementation.

### Type and Size Data Analysis
This section seeks to analyse and discuss the findings of the **Experimental Results - Matrix Size and Type** section.

#### Type
The underlying type implementation of `matTest` was limited by SIMD. I had a lot  of trouble with AVX2, and read through quite a lot of example code and many threads on the topic. Because of the way I implemented SIMD processing, the vectorized load of data takes elements of size 4-bytes, and *only* 4-bytes. Thus, for fixed-point data, I used the standard 4-byte `int`, and for the floating-point data, I used `float` (also 4-bytes).

What this means from a practical perspective is that both `int` and `float` types take approximately the same time to multiply. This is shown numerically with some collected values below:

    Integer Type				        Float Type		
    Size	Naïve	    Max		        Size	Naïve	    Max
    104	    0.005690	0.00201		    104	    0.005633	0.001468
    208	    0.043679	0.004879		208	    0.043752	0.005886
    416	    0.352251	0.022519		416	    0.347601	0.024686

In this table, the `Size` column represents the matrix size, the `Naïve` column represents the time taken to multiply the matrices using the Naive implementation, and the `Max` column represents the time taken to multiply the matrices using the maximally optimized implementation. As one can clearly see, they are within a healthy margin of error. Some of the "larger" differences, such as with size 208 matrices in this figure, can be attributed to minute differences in speed of the fixed- and floating-point processing units within the processor. In the general case however, they `int` and `float` operations simply perform the same. For this reason, separate charts for each type were omitted from the section **Experimental Results - Matrix Size and Type**.

#### Size

Switching gears from type to matrix size, we do observe apparent differences resulting from increased matrix dimensions. The figures in **Experimental Results - Matrix Size and Type** double the matrix dimensions every data point. This means that the total matrix area is increasing quadratically, and we do indeed see a quadratic increase in computation time for both the max and naive implementations over these matrix sizes.

The time required for computation where matrix size is constant is *enormously* larger in the naive implementation. In many of the cases provided in the **Experimental Results - Matrix Size and Type** section, there are over two orders of magnitude separating the computation times of the max and naive approaches. The max efficiency implementation computed the product of two `13312 x 13312` matrices in just over `236 seconds`. For reference, I never even witnessed the naive implementation complete this calculation due to how long it takes with no optimization. This clearly demonstrates the importance of optimizing code, and specifically emphasizes the studied techniques here (*Single Instruction/Multiple Data, MultiThreading, and Cache Access Optimization*). Next, we will analyze the experimental results of these methods compared against one another.

## Optimization Performance Data Analysis
This section seeks to analyse and discuss the findings of the **Optimization Performance Comparison** section.

The centerpiece chart presented in **Optimization Performance Comparison** shows the results of multiplying two `1664 x 1664` integer matrices, for all possible combinations of optimization techniques. The prevailing implementation is, as expected, multiplication where all three optimizations are used, or "`Max`" in the provided chart. This method executes the operation in under half a second (0.495 s). The next fastest method is "`MT & SIMD`" (0.624 s). This is also blazingly fast, and suggests that the most important optimization techniques for the matrix-matrix multiplication task are MultiThreading and SIMD.

This is not to say that cache optimization is unimportant in any way. In fact, one might suggest quite the opposite from this data. The pure Cache Optimization "`CO`" result is almost 10 seconds (28.9%) faster than the naive approach. This is a substantial improvement over baseline for minimal effort the in actual writing of code (cache access is optimized by simply performing matrix transposition for better locality!).

SIMD is the so-called "middle child" of the optimizations presented here (not the best, not the worst). Regardless, it results in a significant decrease in computation time over baseline. Use of AVX2 intrinsics  provides a "`SIMD`" result that is 76.5% faster than the naive approach (crazy good!).

SIMD is very good compared to baseline, however the champion of optimization (and the first on that I implemented) is...

    MultiThreading.

Distributing the multiplication workload over all of the CPU's threads is like taking advantage of completely wasted potential. With purely "`MT`" enabled, we see a ***87.6%*** increase in speed over the naive approach. This was really cool to see, and it makes intuitive sense why the improvement is so large. Using task manager on Windows, I could watch all threads of the processor come under load when running the multithreaded multiplication optimization. Without it, only a few threads were loaded, which completely wasted the others!

## Project Conclusion

This project was quite fun to work on! It taught me quite a bit about optimization, and I learned things that other classes failed to squeeze into their curricula. Granted, some things were very difficult (SIMD.....), but resources like StackOverflow and Intel's AVX Intrinsics manual were very useful to me. Feel free to look at the structure of the source code, I tried to document it pretty thickly with comments. My SIMD implementation was partly thanks to a very useful discussion on StackOverflow that I linked in the comments next to it.

After completing this project, I can say with confidence that I will never underestimate the power of optimization! For workflows which have a high degree of dependence on linear algebra, optimization methods like those studied here are absolutely necessary!