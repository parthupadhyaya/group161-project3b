#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <utility>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

template <typename T>
int partition(vector<T>& arr, int low, int high) {
    static mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(low, high);
    int pivotIndex = dist(gen);
    T pivotValue = arr[pivotIndex];
    swap(arr[pivotIndex], arr[high]);

    int storeIndex = low;
    for (int i = low; i < high; i++) {
        if (arr[i] < pivotValue) {
            swap(arr[i], arr[storeIndex]);
            storeIndex++;
        }
    }
    swap(arr[storeIndex], arr[high]);
    return storeIndex;
}

template <typename T>
void quickSort(vector<T>& arr, int low, int high) {
    if (low < high) {
        int pivotPos = partition(arr, low, high);
        quickSort(arr, low, pivotPos - 1);
        quickSort(arr, pivotPos + 1, high);
    }
}

template <typename T>
void quickSort(vector<T>& arr) {
    if (!arr.empty()) {
        quickSort(arr, 0, static_cast<int>(arr.size()) - 1);
    }
}

// QuickSort with custom comparator for stocks mode
template <typename T, typename Compare>
int partitionC(vector<T>& arr, int low, int high, Compare comp) {
    static mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(low, high);
    int pivotIndex = dist(gen);
    T pivotValue = arr[pivotIndex];
    swap(arr[pivotIndex], arr[high]);
    int storeIndex = low;
    for (int i = low; i < high; i++) {
        if (comp(arr[i], pivotValue)) {
            swap(arr[i], arr[storeIndex]);
            storeIndex++;
        }
    }
    swap(arr[storeIndex], arr[high]);
    return storeIndex;
}

template <typename T, typename Compare>
void quickSortC(vector<T>& arr, int low, int high, Compare comp) {
    if (low < high) {
        int p = partitionC(arr, low, high, comp);
        quickSortC(arr, low, p - 1, comp);
        quickSortC(arr, p + 1, high, comp);
    }
}

template <typename T, typename Compare>
void quickSortC(vector<T>& arr, Compare comp) {
    if (!arr.empty()) {
        quickSortC(arr, 0, static_cast<int>(arr.size()) - 1, comp);
    }
}


int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <mode: quick|stocks> <file1> [file2 ...]" << endl;
        return 1;
    }
    string mode = argv[1];

    if (mode == "quick") {
        ifstream infile(argv[2]);
        if (!infile) {
            cerr << "Can't open file " << argv[2] << endl;
            return 1;
        }
        vector<double> data;
        string line;
        while (getline(infile, line)) {
            stringstream ss(line);
            string token;
            while (getline(ss, token, ',')) {
                stringstream ts(token);
                double value;
                if (ts >> value) data.push_back(value);
            }
        }
        quickSort(data);
        for (const auto& v : data) cout << v << endl;
    }
    else if (mode == "stocks") {
        const double BUY_THRESHOLD = 0.5; // Prob > 50% -> buy
        vector<pair<string, double>> metrics;
        for (int i = 2; i < argc; i++) {
            string path = argv[i];
            string name = path.substr(path.find_last_of("/\\") + 1);
            auto dot = name.find_last_of(".");
            string symbol = (dot == string::npos ? name : name.substr(0, dot));

            ifstream infile(path);

            if (!infile) {
                cerr << "Warning: Couldn't open file " << path << endl;
                continue;
            }
            vector<double> prices;
            string line;
            while (getline(infile, line)) {
                stringstream ss(line);
                string token;
                while (getline(ss, token, ',')) {
                    stringstream ts(token);
                    double price;
                    if (ts >> price) prices.push_back(price);
                }
            }
            int ups = 0;
            for (size_t j = 1; j < prices.size(); j++) {
                if (prices[j] > prices[j - 1]) {
                    ups++;
                }
            }
            double prob = prices.size() > 1 ? static_cast<double>(ups) / (prices.size() - 1) : 0.0;
            metrics.emplace_back(symbol, prob);
        }
        // Use quickSortC to sort metrics by descending prob
        quickSortC(metrics, [](auto& a, auto& b) { return a.second > b.second; });
        for (auto& [symbol, prob] : metrics) {
            string action = (prob > BUY_THRESHOLD ? "BUY" : "AVOID");
            cout << symbol << ", " << fixed << setprecision(2) << prob << ", " << action << endl;
        }
    }
    else {
        cerr << "Unknown mode: " << mode << endl;
        return 1;
    }
    return 0;
}
