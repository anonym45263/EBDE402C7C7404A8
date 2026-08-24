#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>

#include <cstdio>
#include <random>
using namespace std;


const int N = 4747281; // total number of items in Brightkite

void ReadInput(const string& file, vector<long long>& dataset) {
    ifstream in;
    in.open(file);

    tm time{};
    string S, s;
    dataset.reserve(N);

    for (int i = 0; i < N; i++) {
        in >> s >> S >> s >> s >> s;
        memset(&time, 0, sizeof(time));

        // 20250602: add the correct way of using struct tm (year = yy - 1900, mon = mm - 1)
        time.tm_year = atoi(S.c_str()) - 1900;
        time.tm_mon = atoi(S.c_str() + 5) - 1;
        time.tm_mday = atoi(S.c_str() + 8);
        time.tm_hour = atoi(S.c_str() + 11);
        time.tm_min = atoi(S.c_str() + 14);
        time.tm_sec = atoi(S.c_str() + 17);

        dataset.push_back(mktime(&time));
    }

    in.close();
}

template <typename T>
void RandomSample(T start, T end, int num, vector<T>& samples) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<T> dist(start, end);
    
    samples.reserve(num);
    for (int i = 0; i < num; i++) {
        samples.push_back(dist(gen));
    }
}


int main() {
    srand(time(NULL));
    
    cout << "Start reading " << N << " records..." << endl;
    vector<long long> dataset;
    ReadInput("brightkite_totalCheckins.txt", dataset);
    cout << "Dataset size: " << dataset.size() << endl;

    const int frame_size = 60 * 60; // one hour * 24; // one day

    int distr[1000000] = { 0 };
    for (int i = 0; i < N; i++) {
    	long long index = (long long) dataset[i] / (long long) frame_size;
    	distr[index]++;
    }

    vector<long long> distr_starting_data;
    vector<long long> distr_ending_data;
    vector<int> distr_cnt;
    for (int i = 0; i < 1000000; i++) {
    	if (distr[i] > 0) {
    		const long long starting_data = i * frame_size;
    		const long long ending_data = starting_data + frame_size - 1;
    		distr_starting_data.push_back(starting_data);
    		distr_ending_data.push_back(ending_data);
    		distr_cnt.push_back(distr[i]);
    		// cout << starting_data << "-" << ending_data << ": " << distr[i] << endl;
    	}
    }

    cout << "Total number of bins: " << distr_cnt.size() << endl;

    int aug_N = (int) ((double)(1 << 27) * 1.2); // ensure aug-size is more than 128M
    cout << "Augmenting dataset of size: " << aug_N << endl;
    vector<long long> dataset_aug;
	for (int i = 0; i < distr_cnt.size(); i++) {
		int num = (int)((double) distr_cnt[i] * ((double) aug_N / (double) N));

		// cout << distr_starting_data[i] << "-" << distr_ending_data[i] << ": " << num << endl;

		vector<long long> samples;
		RandomSample(distr_starting_data[i], distr_ending_data[i], num, samples);

		for (auto sample: samples) {
			dataset_aug.push_back(sample);
		}
	}

	cout << "Actual num: " << dataset_aug.size() << endl;

	int scale = 0;
    // from 4K to 128M
    for (int scale_N = (1 << 12); scale_N <= (1 << 27); scale_N *= 2) {
    	scale++;
    	string source;
    	if (scale_N < N) {
    		source = "dataset";
    	} else {
    		source = "dataset_aug";
    	}

    	string filename;
    	if (scale < 10) {
    		filename = "brightkite_ts_data_0" + to_string(scale) + ".txt";
    	} else {
    		filename = "brightkite_ts_data_" + to_string(scale) + ".txt";
    	}
    	cout << "Generating dataset of size " << scale_N << " from " << source << " into " << filename << endl;

    	ofstream out(filename);

    	vector<int> samples;
    	if (source == "dataset") {
    		RandomSample((int) 0, (int) (dataset.size() - 1), scale_N, samples);
			for (auto sample: samples) {
				if (sample < 0 || sample >= dataset.size()) {
					cout << sample << endl;
				}
				out << dataset[sample] << endl;
			}
    	} else {
			RandomSample((int) 0, (int) (dataset_aug.size() - 1), scale_N, samples);
			for (auto sample: samples) {
				out << dataset_aug[sample] << endl;
			}
    	}

		out.close();
    }

}