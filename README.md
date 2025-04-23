# **STOCK SORT**

A command-line tool to rank stocks by buy-probability using QuickSort or MergeSort


## **Overview**

Calculates a simple buy-probability for each CSV of historical stock data (probability of an "up" day), then sorts tickers descending by that probability. Offers both QuickSort and MergeSort implementations and reports timing


## **Prerequisites**

- A C++ compiler with C++17 suport (for <filesystem> and <chrono>).
- e.g. GCC 9+, Clang 9+, MSVC 2017+
- CMake (optional, if you prefer generating build files)


## **Building**

### **With CMake**

`mkdir build && cd build`

`cmake ..`

`cmake --build .`

The resulting executable will be named qSort (as per the provided CMakeLists.txt).


### **Without Cmake (GCC/Clang)**

`g++ ../quickmerge.cpp -std=c++17 -O2 -o quickmerge`


### **Without CMake (MSVC)**

`cl /EHsc /std:c++17 quickmerge.cpp`


# **Usage**

`./quickmerge <path-to-csv-directory-or-file>`

1. **Prompt**: The program will ask:

    Choose sort algorithm ('quick' or 'merge'):
2. **Enter** either quick or merge and press Enter;

3. **Output:**

    - Timing of the chosen sort (in microseconds).
    - Sorted list of TICKER, PROBABILITY%, ACTION (BUY if prob > 50%, else AVOID).
   
### **Examples**


**Directory of CSVs**

`./quickmerge C:/Users/ansh_/CLionProjects/StockSort/forbes2000/csv`

Choose sort algorithm ('quick' or 'merge'): quick


**Single CSV file**

`./quickmerge AAPL.csv`

Choose sort algorithm ('quick' or 'merge'): merge


## **CSV Format**

Each CSV should include at least these columns (header row skipped automatically):

**Date, Low, Open, Volume, High, Close, Adjusted Close**

The tool reads the 7th column (Adjusted Close) to compute buy-probability


### **Notes**

    - Files with fewer than two data rows are marked AVOID at 0.00%
    - QuickSort is randomized for average O(n log n), worst-case O(n^2)
    - MergeSort guarntees O(n log n) time and uses O(n) extra space

