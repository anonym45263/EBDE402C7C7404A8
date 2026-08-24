#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <utility>
#include <vector>
#include <random>

#include <ios>
#include <iomanip>
#include <chrono>
#include <sstream>
using namespace std;


const vector<double> range_rate = {0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1};
const int N = 1 << 20;
const int B = 8;
const int REPEAT = 1000;


// const double QRAM_STORE_TIME = 0.0000009;
// const double QRAM_LOAD_TIME =  0.00000932;
// const double GATE_TIME =       0.0000006;


const double QRAM_STORE_TIME = 0.00000009;
const double QRAM_LOAD_TIME =  0.000000932;
const double GATE_TIME =       0.00000006;


struct Measurement {
    double c_query_cl_accum = 0;
    double c_query_cl = 0;

    double t_query_cl_accum = 0;
    double t_query_cl = 0;

    double c_query_qu_accum = 0;
    double c_query_qu= 0;

    double t_query_qu_accum = 0;
    double t_query_qu = 0;

    double c_query_hy_accum = 0;
    double c_query_hy = 0;

    double t_query_hy_accum = 0;
    double t_query_hy = 0;


    void write(ostream& os) {
        os << c_query_cl << "\t" // 2
           << c_query_qu << "\t" // 3
           << c_query_hy << "\t" // 4
           << t_query_cl << "\t" // 5
           << t_query_qu << "\t" // 6
           << t_query_hy; // 7
    }
};

Measurement meas[30];
int meas_i = 0;

void ResetMeasurement() {
    for (int i = 0; i < 30; i++) {
        memset(&meas[i], 0, sizeof(meas[i]));
    }
}

void ComputeMeasurement(int index) {
    meas[index].c_query_cl = meas[index].c_query_cl_accum / double(REPEAT);
    meas[index].t_query_cl = meas[index].t_query_cl_accum / double(REPEAT);

    meas[index].c_query_qu = meas[index].c_query_qu_accum / double(REPEAT);
    meas[index].t_query_qu = meas[index].t_query_qu_accum / double(REPEAT);

    meas[index].c_query_hy = meas[index].c_query_hy_accum / double(REPEAT);
    meas[index].t_query_hy = meas[index].t_query_hy_accum / double(REPEAT);
}

void PrintMeasurements(int scale, int max_print=10) {
    // Save original formatting settings
    ios_base::fmtflags original_flags = cout.flags();
    streamsize original_precision = cout.precision();

    const int NAME_WIDTH = 34;
    const int VALUE_WIDTH = 20;

    // Print rows using helper function
    auto printRow = [&](const string& name, double values[], int begin, int scale, bool print_n = false, double rate_list[] = nullptr) {
        cout << left << setw(NAME_WIDTH) << name;
        for (int i = begin; i < scale; i++) {
            if (print_n) {
                cout << right << setw(VALUE_WIDTH) << rate_list[i];
            } else {
                cout << right << fixed << setprecision(8)
                     << setw(VALUE_WIDTH) << values[i];
                cout.flags(original_flags);
                cout.precision(original_precision);
            }
        }
        cout << endl;
    };

    double values[30];
    double rate_list[30];

    int begin = scale - max_print; // print from begin to scale - 1
    if (begin < 0) { begin = 0; } // e.g., if scale = 8, print 0~7, if scale = 13, print 3~12

    for (int i = begin; i < scale; i++) { rate_list[i] = 0.01 + 0.01 * i; }
    printRow("RATE", values, begin, scale, true, rate_list);

    cout << string(NAME_WIDTH + VALUE_WIDTH * (scale - begin), '-') << endl;


    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_cl; }
    printRow("IO_C Query Classical", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu; }
    printRow("IO_C Query Quantum", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_hy; }
    printRow("IO_C Query Hybrid", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_cl; }
    printRow("Time Query Classical", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu; }
    printRow("Time Query Quantum", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_hy; }
    printRow("Time Query Hybrid", values, begin, scale);

    // Restore original settings
    cout.flags(original_flags);
    cout.precision(original_precision);
}


struct MovieRecord {
    std::string tconst;
    // std::string primaryTitle;
    long long releaseTimestamp;
    double averageRating;
    // int numVotes;
};

std::vector<MovieRecord*> records;

struct Node {
    double* l;
    double* r;
    MovieRecord** d;
    int* c;
    int weight;
    int height;
    bool is_leaf() {
        // assert height > 0
        return (height == 1);
    }
    Node() {
        l = new double[B];
        r = new double[B];
        c = new int[B];
        d = new MovieRecord*[B];
    }
    ~Node() {
        delete[] l;
        delete[] r;
        delete[] c;
        delete[] d;
    }
};

struct Tree {
    Node *node;
    pair<double, MovieRecord*> *data;
    int root; // node index of root in *node
    Tree(int size) {
        node = new Node[size];
        data = new pair<double, MovieRecord*>[size];
    };
    ~Tree() {
        delete[] node;
        delete[] data;
    };
    int size() const {
        return (root + 1);
    }
};

template <typename T>
struct QRAM {
    T *val;
    // T *val_2;
    QRAM(int size) {
        val = new T[size];
        // val_2 = new T[size];
    };
    ~QRAM() {
        delete[] val;
        // delete[] val_2;
    };
    void store(int addr, T v1) {
        val[addr] = v1;
        // val_2[addr] = v2;
    }
};


void ReadInput(const string& file) {
    ifstream in;
    in.open(file);
    std::string line;
    bool firstLine = true;

    while (std::getline(in, line)) {
        if (firstLine) {
            firstLine = false;
            continue;
        }

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(iss, token, '\t')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 5) {
            continue;
        }

        try {
            MovieRecord* record = new MovieRecord;
            record->tconst = tokens[0];
            // record->primaryTitle = tokens[1];
            record->releaseTimestamp = std::stoll(tokens[2]);
            record->averageRating = std::stod(tokens[3]);
            // record->numVotes = std::stoi(tokens[4]);

            records.push_back(record);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << "\n";
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    in.close();
}

void DeleteInput() {
    for (auto record : records) {
        delete record;
    }
    records.clear();
}

void RandomSelectData(int num, vector<MovieRecord*>& selected_records) {
    // select a set of num records randomly from records
    // use a method to select a random set of int from 0 to size(records) without repetition
    // then select the records corresponding to the ints
    selected_records.clear();

    // Handle edge cases
    if (records.empty() || num <= 0) return;
    
    // If num is larger than available records, select all records
    const int M = static_cast<int>(records.size());
    if (num >= M) {
        selected_records = records;
        return;
    }

    // Initialize random number generator
    std::mt19937 rng(std::time(nullptr));
    
    // Create a copy of the record pointers for shuffling
    std::vector<MovieRecord*> shuffled = records;
    
    // Partial Fisher-Yates shuffle: Only shuffle first num elements
    for (int i = 0; i < num; ++i) {
        // Generate random index in [i, M-1]
        std::uniform_int_distribution<int> dist(i, M - 1);
        int j = dist(rng);
        
        // Swap elements at positions i and j
        std::swap(shuffled[i], shuffled[j]);
    }
    
    // Copy first num elements to result
    selected_records.assign(shuffled.begin(), shuffled.begin() + num);
}

int Random(int x) {
    return rand() % x;
}

Tree* BuildTree(const vector<MovieRecord*>& selected_records, int N) {
    auto tree = new Tree(N);
    for (int i = 0; i < N; i++) {
        tree->data[i] = make_pair(selected_records[i]->releaseTimestamp, selected_records[i]);
    }
    sort(tree->data, tree->data + N);
    for (int i = 0; i < N / B; i++) {
        tree->node[i].weight = B;
        tree->node[i].height = 1;
        for (int j = 0; j < B; j++) {
            tree->node[i].c[j] = -1;
            tree->node[i].l[j] = tree->data[i * B + j].first;
            tree->node[i].r[j] = tree->data[i * B + j].first;
            tree->node[i].d[j] = tree->data[i * B + j].second;
        }
    }
    int l = 0;
    int node_cnt = N / B;
    while (node_cnt > l + 1) {
        int r = node_cnt;
        for (int i = 0; i < (r - l) / B; i++) {
            tree->node[node_cnt].weight = tree->node[l + i * B].weight * B;
            tree->node[node_cnt].height = tree->node[l + i * B].height + 1;
            for (int j = 0; j < B; j++) {
                tree->node[node_cnt].c[j] = l + i * B + j;
                tree->node[node_cnt].l[j] = tree->node[l + i * B + j].l[0];
                tree->node[node_cnt].r[j] = tree->node[l + i * B + j].r[B - 1];
            }
            node_cnt++;
        }
        if (r - l < B) {
            tree->node[node_cnt].weight = tree->node[l].weight * (r - l);
            tree->node[node_cnt].height = tree->node[l].height + 1;
            for (int j = l; j < r; j++) {
                tree->node[node_cnt].c[j - l] = j;
                tree->node[node_cnt].l[j - l] = tree->node[j].l[0];
                tree->node[node_cnt].r[j - l] = tree->node[j].r[B - 1];
            }
            for (int j = r - l; j < B; j++) {
                tree->node[node_cnt].l[j] = tree->node[node_cnt].r[j] = -1e9;
            }
            node_cnt++;
        }
        l = r;
    }
    tree->root = node_cnt - 1;
    // cout << "tree->root: " << tree->root << endl;

    return tree;
}

const tuple<QRAM<int>*, QRAM<double>*> BuildTreeQuantum(const Tree* tree) {
    int num_blocks = tree->size() * B;
    auto qram_tree_0 = new QRAM<int>(num_blocks);
    auto qram_tree_1 = new QRAM<double>(num_blocks);

    for (int i = 0; i < tree->size(); i++) {
        for (int j = 0; j < B; j++) {
            int addr = i * B + j;
            if (tree->node[i].is_leaf()) {
                qram_tree_0->store(addr, i);
                qram_tree_1->store(addr, tree->node[i].l[j]);
            } else {
                qram_tree_0->store(addr, tree->node[i].c[j]);
                qram_tree_1->store(addr, -1e9);
            }
        }
    }

    return make_tuple(qram_tree_0, qram_tree_1);
}

int ClassicalQuery(const Tree* tree, double x, double y, int u, vector<MovieRecord*>& result) {
    int cost = 1;
    for (int i = 0; i < B; i++) {
        if ((tree->node[u].l[i] <= y) && (tree->node[u].r[i] >= x)) {
            if (tree->node[u].is_leaf()) {
                result.push_back(tree->node[u].d[i]);
            } else {
                cost += ClassicalQuery(tree, x, y, tree->node[u].c[i], result);
            }
        }
    }
    return cost;
}

int LinearScan(const vector<MovieRecord*>& result) {
    int cost = 0;
    double max_rating = -1e9;
    for (int i = 0; i < result.size(); i++) {
        cost++;
        if (result[i]->averageRating > max_rating) {
            max_rating = result[i]->averageRating;
        }
    }
    return cost;
}

void AppendResult(const Tree* tree, int node_index, double x, double y, std::vector<MovieRecord*>& result) {
    if (tree->node[node_index].is_leaf()) {
        for (int i = 0; i < B; i++) {
            if (tree->node[node_index].l[i] <= y && tree->node[node_index].r[i] >= x) {
                result.push_back(tree->node[node_index].d[i]);
            }
        }
    } else {
        for (int i = 0; i < B; i++) {
            AppendResult(tree, tree->node[node_index].c[i], x, y, result);
        }
    }
}

pair<int, double> QuantumQuery(const Tree* tree, double x, double y, vector<MovieRecord*>& result) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> V;
    V.push_back(tree->root);

    int cost = 0;

    bool stop_flag = false;
    while (true) {
        vector<int> U;
        if (tree->node[V[0]].is_leaf()) {
            stop_flag = true;
            // cout << "Stop since node " << V[0] << " in V are leaves" << endl;
        } else {
            for (int i = 0; i < V.size(); i++) {
                cost++;
                for (int j = 0; j < B; j++) {
                    if ((x <= tree->node[V[i]].l[j]) && (tree->node[V[i]].r[j] <= y)) {
                        stop_flag = true;
                        // cout << "Stop since node " << V[i] << " has inside child" << endl;
                        break;
                    } else if ((x <= tree->node[V[i]].r[j]) && (tree->node[V[i]].l[j] <= y) && (tree->node[V[i]].c[j] != -1)) {
                        U.push_back(tree->node[V[i]].c[j]);
                    }
                }
                if (stop_flag) {
                    break;
                }
            }
        }
        if (stop_flag) {
            break;
        }
        V.clear();
        V = U;
        if (V.size() == 0) {
            break;
        }
    }

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;
    double time = duration.count();

    // cout << "Global classical result ([x, y] = [" << x << ", " << y << "]):" << endl;
    // for (int i = 0; i < V.size(); i++) {
    //     cout << "- Node " << V[i] << " (height = " << tree->node[V[i]].height << "): [" << tree->node[V[i]].l[0] << ", " << tree->node[V[i]].r[B - 1] << "]" << endl;
    // } // could be errorous, print only

    // firstly, do height times of parallel Hadamard gate and QRAM load
    int V_height = tree->node[V[0]].height;

    cost += 2 * V_height;
    time += V_height * QRAM_LOAD_TIME;
    time += V_height * GATE_TIME;

    // // secondly, do post-selection # candidate items / k times
    // // here, we assume that k is already known to obtain the real number of post-selection executions
    // int num_candidates = 0;
    // for (int i = 0; i < V.size(); i++) {
    //     num_candidates += tree->node[V[i]].weight;
    // }
    // int post_sel_times = num_candidates / k;
    int post_sel_times = 8 * B;
    cost += post_sel_times;
    time += post_sel_times * GATE_TIME * 7;

    // obtain all elements in V
    result.clear();
    for (int i = 0; i < V.size(); i++) {
        AppendResult(tree, V[i], x, y, result);
    }

    return make_pair(cost, time);
}

void run_cl(const Tree* tree, double x, double y) {
    vector<MovieRecord*> result;
    auto start = chrono::high_resolution_clock::now();

    int cost_range_query = ClassicalQuery(tree, x, y, tree->root, result);
    meas[meas_i].c_query_cl_accum += double(cost_range_query);

    // cout << "Classical:" << endl;
    // cout << "-> Query range: [" << x << ", " << y << "]" << endl;
    // cout << "-> # answers: " << result.size() << endl;
    // cout << "-> IO cost: " << cost_range_query << endl;
    // cout << "-> IO cost (accumulated): " << meas[meas_i].c_query_cl_accum << endl;
    // cout << "-> Response time: " << duration.count() << "(s)" << endl;
    // cout << "-> Result: [";
    // for (int k = 0; k < 10; k++) {
    //     cout << "(" << result[k]->tconst << ", " << result[k]->primaryTitle << ", " << result[k]->releaseTimestamp << "), ";
    // }
    // cout << "...]" << endl;

    int cost_linear_scan = LinearScan(result);
    meas[meas_i].c_query_cl_accum += double(cost_linear_scan);

    // cout << "LinearScan:" << endl;
    // cout << "-> Query title: " << query_title << endl;
    // cout << "-> IO cost: " << cost_linear_scan << endl;
    // cout << "-> IO cost (accumulated): " << meas[meas_i].c_query_cl_accum << endl;

    // cout << endl;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    meas[meas_i].t_query_cl_accum += duration.count();
}

void run_hy(const Tree* tree, double x, double y) {
    vector<MovieRecord*> result;
    auto start = chrono::high_resolution_clock::now();

    int cost_range_query = ClassicalQuery(tree, x, y, tree->root, result);
    meas[meas_i].c_query_hy_accum += double(cost_range_query);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    meas[meas_i].t_query_hy_accum += duration.count();

    // now, we will simulate using the classical result for quantum maximum
    // first, store all the classical results into the quantum state
    int k = result.size();
    meas[meas_i].c_query_hy_accum += double(k);
    meas[meas_i].t_query_hy_accum += double(k) * GATE_TIME;

    // next, we simulate the quantum algorithm to find the answer
    // for Grover's algorithm, we need to do ceiling(pi/4 * sqrt(k)) times of oracle and diffusion
    // each oracle and diffusion costs (3 * log(k) + 2) * GATE_TIME
    int grover_times = int(ceil(M_PI / 4.0 * sqrt(k)));
    meas[meas_i].c_query_hy_accum += double(grover_times);
    meas[meas_i].t_query_hy_accum += double(grover_times * (3.0 * log(k) / log(2) + 2.0) * GATE_TIME);
}

// void run_hy2(const Tree* tree, double x, double y) {
//     vector<MovieRecord*> result;
//     auto quantum_query_result = QuantumQuery(tree, x, y, result);
//     meas[meas_i].c_query_hy_accum += double(quantum_query_result.first);
//     meas[meas_i].t_query_hy_accum += quantum_query_result.second;

//     // now, we need to add the time and cost for extracting all k results from the superposition
//     // according to existing studies, we need to measure k * (ln(k) + 0.5772156649)
//     int k = result.size();
//     int cost_extract = int(double(k) * (log(k) + 0.5772156649));
//     meas[meas_i].c_query_hy_accum += double(cost_extract);
//     meas[meas_i].t_query_hy_accum += double(cost_extract) * GATE_TIME;

//     auto start = chrono::high_resolution_clock::now();

//     int cost_linear_scan = LinearScan(result);
//     meas[meas_i].c_query_hy_accum += double(cost_linear_scan);

//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double> duration = end - start;
//     meas[meas_i].t_query_hy_accum += duration.count();
// }

void run_qu(const Tree* tree, double x, double y) {
    vector<MovieRecord*> result;
    auto quantum_query_result = QuantumQuery(tree, x, y, result);
    meas[meas_i].c_query_qu_accum += double(quantum_query_result.first);
    meas[meas_i].t_query_qu_accum += quantum_query_result.second;

    // simulate the quantum algorithm to find the answer
    // for Grover's algorithm, we need to do ceiling(pi/4 * sqrt(k)) times of oracle and diffusion
    // each oracle and diffusion costs (3 * log(k) + 2) * GATE_TIME
    int k = result.size();
    int grover_times = int(ceil(M_PI / 4.0 * sqrt(k)));
    meas[meas_i].c_query_qu_accum += double(grover_times);
    meas[meas_i].t_query_qu_accum += double(grover_times * (3.0 * log(k) / log(2) + 2.0) * GATE_TIME);
}

void RandomQuery(const Tree* tree, const pair<double, MovieRecord*>* input, int N, int repeat, double rate) {
    pair<double, MovieRecord*>* input_copy = new pair<double, MovieRecord*>[N];
    for (int i = 0; i < N; i++) {
        input_copy[i] = input[i];
    }
    sort(input_copy, input_copy + N);

    int n = rate * N;
    for (int i = 0; i < repeat; i++) {
        int t = Random(N - n - 1);
        auto x = input_copy[t].first;
        auto y = input_copy[t + n].first;

        run_cl(tree, x, y);
        run_hy(tree, x, y);
        run_qu(tree, x, y);

        // if (i > 3) {
        //     break;
        // }
    }

    delete[] input_copy;
}


int main() {
    srand(12345);
    
    ofstream ofs("plot/IMDb_maximum-v-k.dat");

    streamsize ss = cout.precision();
    cout << setprecision(12);

    ResetMeasurement();

    string filename = "IMDb-processed.txt";
    cout << "Read input from " << filename << "..." << endl;
    ReadInput(filename);
    cout << "Done" << endl;

    vector<MovieRecord*> selected_records;
    RandomSelectData(N, selected_records);

    // for (auto record : selected_records) {
    //     cout << record->tconst << "\t" << record->primaryTitle << "\t" << record->releaseTimestamp << "\t" << record->averageRating << "\t" << record->numVotes << endl;
    // }

    auto tree = BuildTree(selected_records, N);

    auto qram_tuple = BuildTreeQuantum(tree);
    auto qram_tree_0 = get<0>(qram_tuple);
    auto qram_tree_1 = get<1>(qram_tuple);

    for (int i = 0; i < range_rate.size(); i++) {
        double rate = range_rate[i];
        meas_i = i;

        RandomQuery(tree, (const pair<double, MovieRecord*>*) tree->data, N, REPEAT, rate);

        ComputeMeasurement(meas_i);
        PrintMeasurements(meas_i + 1);
        cout << endl;

        ofs << setprecision(8) << rate << "\t";
        meas[meas_i].write(ofs);
        ofs << endl;

        // break;
    }
        
    delete qram_tree_0;
    delete qram_tree_1;
    delete tree;

    cout << setprecision(ss);

    ofs.close();

    cout << "Done" << endl;
    cout.flush();

    DeleteInput();

    return 0;
}
