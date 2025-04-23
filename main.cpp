#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


template <typename T>
void merge(vector<T>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<T> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// Recursive MergeSort on arr[left..right]
template <typename T>
void mergeSort(vector<T>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

template <typename T>
void mergeSort(vector<T>& arr) {
    if (!arr.empty()) mergeSort(arr, 0, static_cast<int>(arr.size()) - 1);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Can't open file " << argv[1] << endl;
        return 1;
    }

    vector<double> data;
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string token;
        while (getline(ss, token, ',')) {
            double value;
            if (stringstream(token) >> value) data.push_back(value);
        }
    }
    mergeSort(data);

    for (const auto& v : data) cout << v << endl;
    return 0;
}