#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <vector>

#include <ios>
#include <iomanip>
#include <chrono>
using namespace std;


double input[3000000];
const int N = 1 << 21; // 2M
const int B = 8;
double RATE;
const int REPEAT = 1000;


const double QRAM_STORE_TIME = 0.0000009;
const double QRAM_LOAD_TIME =  0.00000932;
const double GATE_TIME =       0.0000006;

struct Measurement {
    int    c_query_cl_accum = 0;
    double t_query_cl_accum = 0;
    double c_query_cl = 0;
    double t_query_cl = 0;

    double c_query_qu= 0;
    double t_query_qu = 0;

    int    c_query_qu_gc_accum = 0;
    double t_query_qu_gc_accum = 0;
    double c_query_qu_gc = 0;
    double t_query_qu_gc = 0;

    int    c_query_qu_ql = 0;
    double t_query_qu_ql = 0;

    int    c_query_qu_ql_qram_accum = 0;
    double t_query_qu_ql_qram_accum = 0;
    double c_query_qu_ql_qram = 0;
    double t_query_qu_ql_qram = 0;

    int    c_query_qu_ql_alg_accum = 0;
    double t_query_qu_ql_alg_accum = 0;
    double c_query_qu_ql_alg = 0;
    double t_query_qu_ql_alg = 0;

    void write(ostream& os) {
        os << c_query_cl << "\t" // 2
           << c_query_qu << "\t" // 3
           << c_query_qu_gc << "\t" // 4
           << c_query_qu_ql << "\t" // 5
           << c_query_qu_ql_qram << "\t" // 6
           << c_query_qu_ql_alg << "\t" // 7
           << t_query_cl << "\t" // 8
           << t_query_qu << "\t" // 9
           << t_query_qu_gc << "\t" // 10
           << t_query_qu_ql << "\t" // 11
           << t_query_qu_ql_qram << "\t" // 12
           << t_query_qu_ql_alg; // 13
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

    meas[index].c_query_qu_gc = meas[index].c_query_qu_gc_accum / double(REPEAT);
    meas[index].t_query_qu_gc = meas[index].t_query_qu_gc_accum / double(REPEAT);

    meas[index].c_query_qu_ql_qram = meas[index].c_query_qu_ql_qram_accum / double(REPEAT);
    meas[index].t_query_qu_ql_qram = meas[index].t_query_qu_ql_qram_accum / double(REPEAT);

    meas[index].c_query_qu_ql_alg = meas[index].c_query_qu_ql_alg_accum / double(REPEAT);
    meas[index].t_query_qu_ql_alg = meas[index].t_query_qu_ql_alg_accum / double(REPEAT);

    meas[index].c_query_qu_ql = meas[index].c_query_qu_ql_qram + meas[index].c_query_qu_ql_alg;
    meas[index].t_query_qu_ql = meas[index].t_query_qu_ql_qram + meas[index].t_query_qu_ql_alg;

    meas[index].c_query_qu = meas[index].c_query_qu_gc + meas[index].c_query_qu_ql;
    meas[index].t_query_qu = meas[index].t_query_qu_gc + meas[index].t_query_qu_ql;
}

void PrintMeasurements(int scale, int max_print=10) {
    // Save original formatting settings
    ios_base::fmtflags original_flags = cout.flags();
    streamsize original_precision = cout.precision();

    const int NAME_WIDTH = 34;
    const int VALUE_WIDTH = 20;

    // Print rows using helper function
    auto printRow = [&](const string& name, double values[], int begin, int scale, bool print_rate = false, double rate_list[] = nullptr) {
        cout << left << setw(NAME_WIDTH) << name;
        for (int i = begin; i < scale; i++) {
            if (print_rate) {
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

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu_gc; }
    printRow("- IO_C Query Quantum GC", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu_ql; }
    printRow("- IO_C Query Quantum QL", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu_ql_qram; }
    printRow("--- IO_C Query Quantum GC-QRAM", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu_ql_alg; }
    printRow("--- IO_C Query Quantum GC-Alg", values, begin, scale);


    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_cl; }
    printRow("Time Query Classical", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu; }
    printRow("Time Query Quantum", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu_gc; }
    printRow("- Time Query Quantum GC", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu_ql; }
    printRow("- Time Query Quantum QL", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu_ql_qram; }
    printRow("--- Time Query Quantum GC-QRAM", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu_ql_alg; }
    printRow("--- Time Query Quantum GC-Alg", values, begin, scale);

    // Restore original settings
    cout.flags(original_flags);
    cout.precision(original_precision);
}


struct Node {
    double* l;
    double* r;
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
    }
    ~Node() {
        delete[] l;
        delete[] r;
        delete[] c;
    }
};

struct Tree {
    Node *node;
    double *data;
    int root; // node index of root in *node
    Tree(int size) {
        node = new Node[size];
        data = new double[size];
    };
    ~Tree() {
        delete[] node;
        delete[] data;
    };
    int size() {
        return (root + 1);
    }
};

Tree *tree;

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

QRAM<int> *qram_tree_0;
QRAM<double> *qram_tree_1;


void ReadInput(const string& file) {
    ifstream in;
    in.open(file);

    long long ts_data;
    for (int i = 0; i < N; i++) {
        in >> ts_data;
        input[i] = ts_data;

        // cout << i << ": " << input[i] << endl;
    }

    in.close();
}


int Random(int x) {
    return rand() % x;
}

void BuildTree() {
    tree = new Tree(N);
    memcpy(tree->data, input, sizeof(double) * N);
    sort(tree->data, tree->data + N);
    for (int i = 0; i < N / B; i++) {
        tree->node[i].weight = B;
        tree->node[i].height = 1;
        for (int j = 0; j < B; j++) {
            tree->node[i].c[j] = -1;
            tree->node[i].l[j] = tree->data[i * B + j];
            tree->node[i].r[j] = tree->data[i * B + j];
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
}

void BuildTreeQuantum() {
    int num_blocks = tree->size() * B;
    qram_tree_0 = new QRAM<int>(num_blocks);
    qram_tree_1 = new QRAM<double>(num_blocks);

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
}

int ClassicalQuery(double x, double y, int u, vector<double>& result) {
    meas[meas_i].c_query_cl_accum++;
    int ans = 0;
    for (int i = 0; i < B; i++) {
        if ((tree->node[u].l[i] <= y) && (tree->node[u].r[i] >= x)) {
            if (tree->node[u].is_leaf()) {
                ans++;
                result.push_back(tree->node[u].l[i]);
            } else {
                ans += ClassicalQuery(x, y, tree->node[u].c[i], result);
            }
        }
    }
    return ans;
}

void QuantumQuery(double x, double y, int k) {
    auto start = chrono::high_resolution_clock::now();

    vector<int> V;
    V.push_back(tree->root);

    bool stop_flag = false;
    while (true) {
        vector<int> U;
        if (tree->node[V[0]].is_leaf()) {
            stop_flag = true;
            // cout << "Stop since node " << V[0] << " in V are leaves" << endl;
        } else {
            for (int i = 0; i < V.size(); i++) {
                meas[meas_i].c_query_qu_gc_accum++;
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
    meas[meas_i].t_query_qu_gc_accum += duration.count();

    // cout << "Global classical result ([x, y] = [" << x << ", " << y << "]):" << endl;
    // for (int i = 0; i < V.size(); i++) {
    //     cout << "- Node " << V[i] << " (height = " << tree->node[V[i]].height << "): [" << tree->node[V[i]].l[0] << ", " << tree->node[V[i]].r[B - 1] << "]" << endl;
    // } // could be errorous, print only

    // firstly, do height times of parallel Hadamard gate and QRAM load
    int V_height = tree->node[V[0]].height;

    meas[meas_i].c_query_qu_ql_qram_accum += V_height;
    meas[meas_i].c_query_qu_ql_alg_accum += V_height;

    meas[meas_i].t_query_qu_ql_qram_accum += V_height * QRAM_LOAD_TIME;
    meas[meas_i].t_query_qu_ql_alg_accum += V_height * GATE_TIME;

    // // secondly, do post-selection # candidate items / k times
    // // here, we assume that k is already known to obtain the real number of post-selection executions
    // int num_candidates = 0;
    // for (int i = 0; i < V.size(); i++) {
    //     num_candidates += tree->node[V[i]].weight;
    // }
    // int post_sel_times = num_candidates / k;
    int post_sel_times = 8 * B;
    meas[meas_i].c_query_qu_ql_alg_accum += post_sel_times;
    meas[meas_i].t_query_qu_ql_alg_accum += post_sel_times * GATE_TIME * 7;
}

void RandomQuery(int repeat, double rate) {
    sort(input, input + N);
    // double l = input[N / 10];
    // double r = input[N / 10 * 9];
    int n = rate * N;
    for (int i = 0; i < repeat; i++) {
        int t = Random(N - n - 1);
        double x = input[t];
        double y = input[t + n];

        vector<double> result;
        auto start = chrono::high_resolution_clock::now();
        int k = ClassicalQuery(x, y, tree->root, result);
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> duration = end - start;
        meas[meas_i].t_query_cl_accum += duration.count();

        // cout << "Classical:" << endl;
        // cout << "-> # answers: " << k << endl;
        // cout << "-> IO cost: " << meas[meas_i].c_query_cl_accum << endl;
        // cout << "-> Response time: " << duration.count() << "(s)" << endl;
        // cout << "-> Result: [";
        // for (int k = 0; k < 10; k++) {
        //     cout << result[k] << ", ";
        // }
        // cout << "...]" << endl;

        QuantumQuery(x, y, k);

    }
}

int main() {
    srand(time(NULL));
    
    // ReadInput("loc-brightkite_totalCheckins.txt");

    ofstream ofs("plot/Brightkite-v-k.sta");

    streamsize ss = cout.precision();
    cout << setprecision(12);

    string filename = "brightkite_ts_data_10.txt";

    cout << "Read input from " << filename << "..." << endl;
    ReadInput(filename);
    cout << "Done" << endl;

    BuildTree();
    BuildTreeQuantum();

    int scale = 0;
    ResetMeasurement();

    for (RATE = 0.01; RATE <= 0.1001; RATE += 0.01) {
        scale++;
        meas_i = scale - 1;

        RandomQuery(REPEAT, RATE);

        ComputeMeasurement(meas_i);
        PrintMeasurements(scale);
        cout << endl;

        ofs << setprecision(8) << RATE << "\t";
        meas[meas_i].write(ofs);
        ofs << endl;

        // if (scale > 3) break;
    }
        
    delete qram_tree_0;
    delete qram_tree_1;
    delete tree;

    cout << setprecision(ss);

    ofs.close();


    return 0;
}
