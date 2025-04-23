#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
// Remove <filesystem> if you don’t have libstdc++fs support
// #include <filesystem>
// namespace fs = std::filesystem;

using namespace std;

//— Merge with comparator —//
template<typename T, typename Compare>
void mergeC(vector<T>& arr, int left, int mid, int right, Compare comp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<T> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (comp(L[i], R[j]))
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

template<typename T, typename Compare>
void mergeSortC(vector<T>& arr, int left, int right, Compare comp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortC(arr, left,  mid,     comp);
        mergeSortC(arr, mid+1, right,   comp);
        mergeC(arr, left,  mid,     right, comp);
    }
}

template<typename T, typename Compare>
void mergeSortC(vector<T>& arr, Compare comp) {
    if (!arr.empty())
        mergeSortC(arr, 0, static_cast<int>(arr.size()) - 1, comp);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path-to-csv-or-directory>" << endl;
        return 1;
    }

    // If you need <filesystem>, uncomment and fix your CMake accordingly.
    // fs::path input(argv[1]);
    vector<string> paths;
    // If argv[1] is a directory, you'd scan it here.
    // For now, we’ll just treat each argv as a file:
    for (int i = 1; i < argc; ++i)
        paths.push_back(argv[i]);

    const double BUY_THRESHOLD = 0.5;
    vector<pair<string,double>> metrics;

    for (auto& path: paths) {
        // Extract symbol from filename:
        auto slash = path.find_last_of("/\\");
        string name = (slash==string::npos ? path : path.substr(slash+1));
        auto dot = name.find_last_of('.');
        string symbol = (dot==string::npos ? name : name.substr(0,dot));

        ifstream infile(path);
        if (!infile) {
            cerr << "Warning: Could not open file " << path << endl;
            continue;
        }

        vector<double> prices;
        string line;
        getline(infile, line);               // skip header
        while (getline(infile, line)) {
            stringstream ss(line);
            vector<string> cols;
            string token;
            while (getline(ss, token, ','))
                cols.push_back(token);
            if (cols.size() >= 7) {
                try {
                    prices.push_back(stod(cols[6]));
                } catch (...) { }
            }
        }

        double prob = 0.0;
        if (prices.size() < 2) {
            cerr << "Warning: Not enough data in " << symbol << endl;
        } else {
            int ups = 0;
            for (size_t j = 1; j < prices.size(); ++j)
                if (prices[j] > prices[j-1])
                    ++ups;
            prob = static_cast<double>(ups) / (prices.size() - 1);
        }
        metrics.emplace_back(symbol, prob);
    }

    // Sort descending by probability
    mergeSortC(metrics, [](auto& a, auto& b){
        return a.second > b.second;
    });

    // Print results
    for (auto& [symbol, prob] : metrics) {
        string action = (prob > BUY_THRESHOLD ? "BUY" : "AVOID");
        cout << symbol << ", "
             << fixed << setprecision(2)
             << (prob * 100) << "%, "
             << action << "\n";
    }

    return 0;
}
