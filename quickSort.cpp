#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


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

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path-to-csv-or-directory>" << endl;
        return 1;
    }

    // Collect CSV file paths: accept directory or individual file paths.
    vector<string> paths;
    fs::path input(argv[1]);
    if (fs::exists(input) && fs::is_directory(input)) {
        for (auto& entry : fs::directory_iterator(input)) {
            if (entry.is_regular_file() && entry.path().extension() == ".csv")
                paths.push_back(entry.path().string());
        }
    } else {
        for (int i = 1; i < argc; i++)
            paths.push_back(argv[i]);
    }

    // Compute buy probability for each stock file
    const double BUY_THRESHOLD = 0.5;
    vector<pair<string, double>> metrics;
    for (auto& path : paths) {
        string symbol = fs::path(path).stem().string();
        ifstream infile(path);
        if (!infile) {
            cerr << "Warning: Could not open file " << path << endl;
            continue;
        }
        vector<double> prices;
        string line;
        getline(infile, line);
        while (getline(infile, line)) {
            vector<string> cols;
            string token;
            stringstream ss(line);
            while (getline(ss, token, ',')) cols.push_back(token);
            if (cols.size() > 7) {
                try {
                    double price = stod(cols[6]);
                    prices.push_back(price);
                } catch (...) {}
            }
        }

        if (prices.size() < 2) {
            cerr << "Warning: Not enough data in " << symbol << endl;
            metrics.emplace_back(symbol, 0.0);
            continue;
        }
        int ups = 0;
        for (size_t j = 1; j < prices.size(); j++)
            if (prices[j] < prices[j - 1]) ups++;
        double prob = prices.size() > 1 ? static_cast<double>(ups) / (prices.size() - 1) : 0.0;
        metrics.emplace_back(symbol, prob);
    }

    // Sort stocks by descending buy probability
    quickSortC(metrics, [](auto& a, auto& b) { return a.second > b.second; });

    // Output sorted list with action reccomendation
    for (auto& [symbol, prob] : metrics) {
        string action = (prob > BUY_THRESHOLD ? "BUY" : "AVOID");
        cout << symbol << ", " << fixed << setprecision(2) << (prob * 100) << "%, " << action << endl;
    }
    return 0;
}
