#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <utility>
#include <random>

double Random() {
    return rand() / (double)RAND_MAX;
}

// const std::string computeHash(const std::string& key) {
//     // std::hash<std::string> hash_fn;
//     // return std::to_string(hash_fn(key));

//     std::string result = key;
//     for (int round = 0; round < 2; round++) {
//         std::string temp = result;
//         for (size_t i = 0; i < temp.length(); i++) {
//             temp[i] = temp[i] ^ ((i + round) % 256);
//             temp[i] = (temp[i] << 3) | (temp[i] >> 5);
//         }
//         result = temp;
//     }
//     std::hash<std::string> hash_fn;
//     return std::to_string(hash_fn(result));
// }

// Function to calculate the hash of a string
const std::string computeHash(const std::string& str) {
    unsigned long hash = 5381; // Starting value (seed) for the hash
    for (char c : str) {
        // Multiply hash by 33 and add the ASCII value of the character
        hash = ((hash << 5) + hash) + c; // Equivalent to: hash * 33 + c
    }

    // Convert the hash value to a hexadecimal string
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash; // 16 chars for consistent width

    std::hash<std::string> hash_fn;
    return std::to_string(hash_fn(ss.str()));
}

// Utility function to split a line into tokens based on the '|' delimiter
std::vector<std::string> split(const std::string& line, char delimiter = '|') {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert a date string (YYYY-MM-DD) to the number of days since 1970-01-01
int dateToDays(const std::string& date) {
    if (date.empty()) return 0;

    std::tm time = {};
    time.tm_year = std::stoi(date.substr(0, 4)) - 1900;  // Year since 1900
    time.tm_mon = std::stoi(date.substr(5, 2)) - 1;      // Month (0-based)
    time.tm_mday = std::stoi(date.substr(8, 2));         // Day

    std::time_t timeSinceEpoch = std::mktime(&time);
    return static_cast<int>(timeSinceEpoch / (60 * 60 * 24));  // Convert seconds to days
}

// Function to compute a histogram for a single numerical field
template <typename T>
void computeHistogram(const std::vector<T>& data, const std::string& fieldName, double (T::*field), int numBins = 50) {
    if (data.empty()) {
        std::cout << fieldName << ": No data available" << std::endl;
        return;
    }

    // Find min and max values
    double minVal = std::numeric_limits<double>::max();
    double maxVal = std::numeric_limits<double>::lowest();

    for (const auto& row : data) {
        minVal = std::min(minVal, row.*field);
        maxVal = std::max(maxVal, row.*field);
    }

    // Create histogram bins
    std::vector<int> histogram(numBins, 0);
    double binWidth = (maxVal - minVal) / numBins;
    
    if (binWidth == 0) {
        std::cout << fieldName << ": All values are identical (" << minVal << ")" << std::endl;
        return;
    }

    // Fill histogram
    for (const auto& row : data) {
        int binIndex = static_cast<int>((row.*field - minVal) / binWidth);
        if (binIndex >= numBins) binIndex = numBins - 1; // Handle edge case
        histogram[binIndex]++;
    }

    // Print histogram
    std::cout << fieldName << " histogram [" << minVal << ", " << maxVal << "] with " << numBins << " bins:" << std::endl;
    for (int i = 0; i < numBins; i++) {
        double binStart = minVal + i * binWidth;
        double binEnd = minVal + (i + 1) * binWidth;
        std::cout << "  [" << std::fixed << std::setprecision(2) << binStart 
                  << ", " << std::fixed << std::setprecision(2) << binEnd 
                  << "): " << histogram[i] << " items" << std::endl;
    }
    std::cout << std::endl;
}

// Function to compute a histogram for date fields
template <typename T>
void computeHistogram(const std::vector<T>& data, const std::string& fieldName, int (T::*field), int numBins = 50) {
    if (data.empty()) {
        std::cout << fieldName << ": No data available" << std::endl;
        return;
    }

    // Find min and max values
    int minVal = std::numeric_limits<int>::max();
    int maxVal = std::numeric_limits<int>::lowest();

    for (const auto& row : data) {
        minVal = std::min(minVal, row.*field);
        maxVal = std::max(maxVal, row.*field);
    }

    // Create histogram bins
    std::vector<int> histogram(numBins, 0);
    double binWidth = static_cast<double>(maxVal - minVal) / numBins;
    
    if (binWidth == 0) {
        std::cout << fieldName << ": All values are identical (" << minVal << ")" << std::endl;
        return;
    }

    // Fill histogram
    for (const auto& row : data) {
        int binIndex = static_cast<int>((row.*field - minVal) / binWidth);
        if (binIndex >= numBins) binIndex = numBins - 1; // Handle edge case
        histogram[binIndex]++;
    }

    // Print histogram
    std::cout << fieldName << " histogram [" << minVal << ", " << maxVal << "] with " << numBins << " bins:" << std::endl;
    for (int i = 0; i < numBins; i++) {
        int binStart = minVal + static_cast<int>(i * binWidth);
        int binEnd = minVal + static_cast<int>((i + 1) * binWidth);
        std::cout << "  [" << binStart << ", " << binEnd 
                  << "): " << histogram[i] << " items" << std::endl;
    }
    std::cout << std::endl;
}

// Struct Definitions with Embedded Parse Functions
struct Customer {
    int custkey;
    std::string name;
    std::string address;
    int nationkey;
    std::string phone;
    double acctbal;
    std::string mktsegment;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Customer& row) {
        row.custkey = std::stoi(tokens[0]);
        row.name = tokens[1];
        row.address = tokens[2];
        row.nationkey = std::stoi(tokens[3]);
        row.phone = tokens[4];
        row.acctbal = std::stod(tokens[5]);
        row.mktsegment = tokens[6];
        row.comment = tokens[7];
    }
};

struct LineItem {
    int orderkey;
    int partkey;
    int suppkey;
    int linenumber;
    int quantity;
    double extendedprice;
    double discount;
    double tax;
    char returnflag;
    char linestatus;
    int shipdate;
    int commitdate;
    int receiptdate;
    std::string shipinstruct;
    std::string shipmode;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, LineItem& row) {
        row.orderkey = std::stoi(tokens[0]);
        row.partkey = std::stoi(tokens[1]);
        row.suppkey = std::stoi(tokens[2]);
        row.linenumber = std::stoi(tokens[3]);
        row.quantity = std::stoi(tokens[4]);
        row.extendedprice = std::stod(tokens[5]);
        row.discount = std::stod(tokens[6]);
        row.tax = std::stod(tokens[7]);
        row.returnflag = tokens[8][0];
        row.linestatus = tokens[9][0];
        row.shipdate = dateToDays(tokens[10]);
        row.commitdate = dateToDays(tokens[11]);
        row.receiptdate = dateToDays(tokens[12]);
        row.shipinstruct = tokens[13];
        row.shipmode = tokens[14];
        row.comment = tokens[15];
    }
};

struct Nation {
    int nationkey;
    std::string name;
    int regionkey;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Nation& row) {
        row.nationkey = std::stoi(tokens[0]);
        row.name = tokens[1];
        row.regionkey = std::stoi(tokens[2]);
        row.comment = tokens[3];
    }
};

struct Orders {
    int orderkey;
    int custkey;
    char orderstatus;
    double totalprice;
    int orderdate;
    std::string orderpriority;
    std::string clerk;
    int shippriority;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Orders& row) {
        row.orderkey = std::stoi(tokens[0]);
        row.custkey = std::stoi(tokens[1]);
        row.orderstatus = tokens[2][0];
        row.totalprice = std::stod(tokens[3]);
        row.orderdate = dateToDays(tokens[4]);
        row.orderpriority = tokens[5];
        row.clerk = tokens[6];
        row.shippriority = std::stoi(tokens[7]);
        row.comment = tokens[8];
    }
};

struct Part {
    int partkey;
    std::string name;
    std::string mfgr;
    std::string brand;
    std::string type;
    int size;
    std::string container;
    double retailprice;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Part& row) {
        row.partkey = std::stoi(tokens[0]);
        row.name = tokens[1];
        row.mfgr = tokens[2];
        row.brand = tokens[3];
        row.type = tokens[4];
        row.size = std::stoi(tokens[5]);
        row.container = tokens[6];
        row.retailprice = std::stod(tokens[7]);
        row.comment = tokens[8];
    }
};

struct PartSupp {
    int partkey;
    int suppkey;
    int availqty;
    double supplycost;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, PartSupp& row) {
        row.partkey = std::stoi(tokens[0]);
        row.suppkey = std::stoi(tokens[1]);
        row.availqty = std::stoi(tokens[2]);
        row.supplycost = std::stod(tokens[3]);
        row.comment = tokens[4];
    }
};

struct Region {
    int regionkey;
    std::string name;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Region& row) {
        row.regionkey = std::stoi(tokens[0]);
        row.name = tokens[1];
        row.comment = tokens[2];
    }
};

struct Supplier {
    int suppkey;
    std::string name;
    std::string address;
    int nationkey;
    std::string phone;
    double acctbal;
    std::string comment;

    static void parse(const std::vector<std::string>& tokens, Supplier& row) {
        row.suppkey = std::stoi(tokens[0]);
        row.name = tokens[1];
        row.address = tokens[2];
        row.nationkey = std::stoi(tokens[3]);
        row.phone = tokens[4];
        row.acctbal = std::stod(tokens[5]);
        row.comment = tokens[6];
    }
};

// Data storage
std::vector<Customer> customers;
std::vector<LineItem> lineitems;
std::vector<Nation> nations;
std::vector<Orders> orders;
std::vector<Part> parts;
std::vector<PartSupp> partsupps;
std::vector<Region> regions;
std::vector<Supplier> suppliers;

// Function to read data from a .tbl file into a vector of structs
template <typename T>
int readTable(const std::string& filename, std::vector<T>& table) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    int lineCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Split the line into tokens
        std::vector<std::string> tokens = split(line);

        // Parse tokens into a struct
        T row;
        T::parse(tokens, row);

        // Add the struct to the table
        table.push_back(row);
        ++lineCount;
    }
    file.close();
    return lineCount;
}

// const double QRAM_STORE_TIME = 0.0000009;
// const double QRAM_LOAD_TIME =  0.00000932;
// const double GATE_TIME =       0.0000006;

const double QRAM_STORE_TIME = 0.00000009;
const double QRAM_LOAD_TIME =  0.000000932;
const double GATE_TIME =       0.00000006;

const int B = 8;
const int REPEAT = 100;
const std::vector<double> SEL_LIST = {0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1};

struct Measurement {
    double c_query_cl_accum = 0;
    double c_query_cl = 0;

    double c_query_cl_op_accum = 0;
    double c_query_cl_op = 0;
    double c_query_cl_pr_accum = 0;
    double c_query_cl_pr = 0;

    double t_query_cl_accum = 0;
    double t_query_cl = 0;

    double t_query_cl_op_accum = 0;
    double t_query_cl_op = 0;
    double t_query_cl_pr_accum = 0;
    double t_query_cl_pr = 0;
    
    double c_query_qu_accum = 0;
    double c_query_qu= 0;

    double c_query_qu_op_accum = 0;
    double c_query_qu_op = 0;
    double c_query_qu_pr_accum = 0;
    double c_query_qu_pr = 0;

    double t_query_qu_accum = 0;
    double t_query_qu = 0;

    double t_query_qu_op_accum = 0;
    double t_query_qu_op = 0;
    double t_query_qu_pr_accum = 0;
    double t_query_qu_pr = 0;

    double c_query_es_accum = 0;
    double c_query_es = 0;

    double c_query_es_op_accum = 0;
    double c_query_es_op = 0;
    double c_query_es_pr_accum = 0;
    double c_query_es_pr = 0;

    double t_query_es_accum = 0;
    double t_query_es = 0;

    double t_query_es_op_accum = 0;
    double t_query_es_op = 0;
    double t_query_es_pr_accum = 0;
    double t_query_es_pr = 0;

    void write(std::ostream& os) {
        os << c_query_cl << "\t" // 2
           << c_query_cl_op << "\t" // 3
           << c_query_cl_pr << "\t" // 4
           << c_query_qu << "\t" // 5
           << c_query_qu_op << "\t" // 6
           << c_query_qu_pr << "\t" // 7
           << c_query_es << "\t" // 8
           << c_query_es_op << "\t" // 9
           << c_query_es_pr << "\t" // 10
           << t_query_cl << "\t" // 5
           << t_query_cl_op << "\t" // 6
           << t_query_cl_pr << "\t" // 7
           << t_query_qu << "\t" // 8
           << t_query_qu_op << "\t" // 9
           << t_query_qu_pr << "\t" // 10
           << t_query_es << "\t" // 11
           << t_query_es_op << "\t" // 12
           << t_query_es_pr; // 13
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

    meas[index].c_query_cl_op = meas[index].c_query_cl_op_accum / double(REPEAT);
    meas[index].t_query_cl_op = meas[index].t_query_cl_op_accum / double(REPEAT);
    meas[index].c_query_cl_pr = meas[index].c_query_cl_pr_accum / double(REPEAT);
    meas[index].t_query_cl_pr = meas[index].t_query_cl_pr_accum / double(REPEAT);

    meas[index].c_query_qu = meas[index].c_query_qu_accum / double(REPEAT);
    meas[index].t_query_qu = meas[index].t_query_qu_accum / double(REPEAT);

    meas[index].c_query_qu_op = meas[index].c_query_qu_op_accum / double(REPEAT);
    meas[index].t_query_qu_op = meas[index].t_query_qu_op_accum / double(REPEAT);
    meas[index].c_query_qu_pr = meas[index].c_query_qu_pr_accum / double(REPEAT);
    meas[index].t_query_qu_pr = meas[index].t_query_qu_pr_accum / double(REPEAT);

    meas[index].c_query_es = meas[index].c_query_es_accum / double(REPEAT);
    meas[index].t_query_es = meas[index].t_query_es_accum / double(REPEAT);

    meas[index].c_query_es_op = meas[index].c_query_es_op_accum / double(REPEAT);
    meas[index].t_query_es_op = meas[index].t_query_es_op_accum / double(REPEAT);
    meas[index].c_query_es_pr = meas[index].c_query_es_pr_accum / double(REPEAT);
    meas[index].t_query_es_pr = meas[index].t_query_es_pr_accum / double(REPEAT);
}

void PrintMeasurements(int scale, int max_print=10) {
    // Save original formatting settings
    std::ios_base::fmtflags original_flags = std::cout.flags();
    std::streamsize original_precision = std::cout.precision();

    const int NAME_WIDTH = 34;
    const int VALUE_WIDTH = 20;

    // Print rows using helper function
    auto printRow = [&](const std::string& name, double values[], int begin, int scale, bool print_sel = false, double sel_list[] = nullptr) {
        std::cout << std::left << std::setw(NAME_WIDTH) << name;
        for (int i = begin; i < scale; i++) {
            if (print_sel) {
                std::cout << std::right << std::setw(VALUE_WIDTH) << sel_list[i];
            } else {
                std::cout << std::right << std::fixed << std::setprecision(8)
                     << std::setw(VALUE_WIDTH) << values[i];
                std::cout.flags(original_flags);
                std::cout.precision(original_precision);
            }
        }
        std::cout << std::endl;
    };

    double values[30];
    double sel_list[30];

    int begin = scale - max_print; // print from begin to scale - 1
    if (begin < 0) { begin = 0; } // e.g., if scale = 8, print 0~7, if scale = 13, print 3~12

    for (int i = begin; i < scale; i++) { sel_list[i] = SEL_LIST[i]; }
    printRow("Selectivity", values, begin, scale, true, sel_list);

    std::cout << std::string(NAME_WIDTH + VALUE_WIDTH * (scale - begin), '-') << std::endl;


    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_cl; }
    printRow("IO_C Query Classical", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_qu; }
    printRow("IO_C Query Quantum", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].c_query_es; }
    printRow("IO_C Query Estimation", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_cl; }
    printRow("Time Query Classical", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_qu; }
    printRow("Time Query Quantum", values, begin, scale);

    for (int i = begin; i < scale; i++) { values[i] = meas[i].t_query_es; }
    printRow("Time Query Estimation", values, begin, scale);

    // Restore original settings
    std::cout.flags(original_flags);
    std::cout.precision(original_precision);
}

struct Node {
    double* l;
    double* r;
    Orders** d;
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
        d = new Orders*[B];
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
    std::pair<double, Orders*> *data;
    int root; // node index of root in *node
    Tree(int size) {
        node = new Node[size];
        data = new std::pair<double, Orders*>[size];
    };
    ~Tree() {
        delete[] node;
        delete[] data;
    };
    int size() const {
        return (root + 1);
    }
};

Tree* BuildTree(std::vector<Orders>& records, int N) {
    auto tree = new Tree(N);
    for (int i = 0; i < N; i++) {
        tree->data[i] = std::make_pair(records[i].totalprice, &records[i]);
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

int ClassicalQuery(const Tree* tree, double x, double y, int u, std::vector<Orders*>& result) {
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

void AppendResult(const Tree* tree, int node_index, double x, double y, std::vector<Orders*>& result) {
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

std::pair<int, double> QuantumQuery(const Tree* tree, double x, double y, std::vector<Orders*>& result) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> V;
    V.push_back(tree->root);

    int cost = 0;

    bool stop_flag = false;
    while (true) {
        std::vector<int> U;
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

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    double time = duration.count();

    // std::cout << "Global classical result ([x, y] = [" << x << ", " << y << "]):" << std::endl;
    // for (int i = 0; i < V.size(); i++) {
    //     std::cout << "- Node " << V[i] << " (height = " << tree->node[V[i]].height << "): [" << tree->node[V[i]].l[0] << ", " << tree->node[V[i]].r[B - 1] << "]" << std::endl;
    // } // could be errorous, print only

    // firstly, do height times of parallel Hadamard gate and QRAM load
    int V_height = tree->node[V[0]].height;

    cost += 2 * V_height;
    time += V_height * QRAM_LOAD_TIME;
    time += V_height * GATE_TIME;

    // secondly, do a simplified post-selection to obtain the number of candidates only
    // it needs around O(sqrt(8*B)) times of gate execution
    int count_cost = int(sqrt(8 * B));
    cost += count_cost;
    time += count_cost * GATE_TIME * 7;

    // obtain all elements in V
    result.clear();
    for (int i = 0; i < V.size(); i++) {
        AppendResult(tree, V[i], x, y, result);
    }

    return std::make_pair(cost, time);
}

// std::pair<int, double> performNestLoopJoin(const double x, const double y, const std::vector<Orders>& orders, const std::vector<LineItem>& lineitems) {
//     auto start = std::chrono::high_resolution_clock::now();
//     int cost = 0;
//     // find the orders with totalprice between x and y
//     std::vector<Orders> orders_in_range;
//     orders_in_range.clear();
//     for (const auto& order : orders) {
//         if (order.totalprice >= x && order.totalprice <= y) {
//             orders_in_range.push_back(order);
//         }
//     }
//     cost += int(log2(orders.size()));
//     // std::cout << "Number of orders in range: " << orders_in_range.size() << std::endl;
//     // for (const auto& order : orders_in_range) {
//     //     std::cout << "Order: " << order.orderkey << " with totalprice " << order.totalprice << std::endl;
//     // }

//     // perform a nest loop join between orders and lineitems, where order.totalprice is between x and y
//     // std::cout << "Performing nest loop join" << std::endl;

//     std::vector<LineItem> results;
//     results.clear();

//     for (const auto& order : orders_in_range) {
//         for (const auto& lineitem : lineitems) {
//             if (lineitem.orderkey == order.orderkey) {
//                 results.push_back(lineitem);
//                 // std::cout << "Found a lineitem: " << lineitem.orderkey << " of orderkey " << order.orderkey << std::endl;
//                 cost++;
//             }
//         }
//     }
//     // std::cout << "Number of lineitems in results: " << results.size() << std::endl;
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> duration = end - start;
//     // std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

//     return std::make_pair(cost, duration.count());
// }

std::pair<int, double> joinWithPartsupp(const double x, const double y, const std::vector<PartSupp>& partsupps, const std::vector<LineItem>& lineitems, std::vector<LineItem>& results) {
    auto start = std::chrono::high_resolution_clock::now();
    int cost = 0;
    // find the orders with totalprice between x and y
    std::vector<PartSupp> partsupps_in_range;
    partsupps_in_range.clear();
    for (const auto& partsupp : partsupps) {
        if (partsupp.availqty >= x && partsupp.availqty <= y) {
            partsupps_in_range.push_back(partsupp);
        }
    }
    cost += int(log2(partsupps.size()));
    std::cout << "Number of partsupps in range: " << partsupps_in_range.size() << std::endl;

    results.clear();

    // build a hash table for orders_in_range according to orderkey
    std::unordered_map<long long, PartSupp> partsupps_hash_table;
    partsupps_hash_table.clear();
    for (const auto& partsupp : partsupps_in_range) {
        partsupps_hash_table[partsupp.partkey * 10000 + partsupp.suppkey] = partsupp;
        cost++;
    }

    // perform a hash join between orders_hash_table and lineitems
    if (partsupps_hash_table.size() > 0) {
        for (const auto& lineitem : lineitems) {
            cost++;
            if (partsupps_hash_table.find(lineitem.partkey * 10000 + lineitem.suppkey) != partsupps_hash_table.end()) {
                results.push_back(lineitem);
                // std::cout << "Found a lineitem: " << lineitem.orderkey << " of order hash key " << computeHash(std::to_string(lineitem.orderkey)) << " and orderkey " << lineitem.orderkey << std::endl;
            }
        }
    }
    // std::cout << "Number of lineitems in results: " << results.size() << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    return std::make_pair(cost, duration.count());
}

std::pair<int, double> joinWithOrders(const double x, const double y, const std::vector<Orders>& orders, const std::vector<LineItem>& lineitems, std::vector<LineItem>& results) {
    auto start = std::chrono::high_resolution_clock::now();
    int cost = 0;
    // find the orders with totalprice between x and y
    std::vector<Orders> orders_in_range;
    orders_in_range.clear();
    for (const auto& order : orders) {
        if (order.totalprice >= x && order.totalprice <= y) {
            orders_in_range.push_back(order);
        }
    }
    cost += int(log2(orders.size()));

    results.clear();

    // build a hash table for orders_in_range according to orderkey
    std::unordered_map<int, Orders> orders_hash_table;
    orders_hash_table.clear();
    for (const auto& order : orders_in_range) {
        orders_hash_table[order.orderkey] = order;
        cost++;
    }

    // perform a hash join between orders_hash_table and lineitems
    if (orders_hash_table.size() > 0) {
        for (const auto& lineitem : lineitems) {
            cost++;
            if (orders_hash_table.find(lineitem.orderkey) != orders_hash_table.end()) {
                results.push_back(lineitem);
                // std::cout << "Found a lineitem: " << lineitem.orderkey << " of order hash key " << computeHash(std::to_string(lineitem.orderkey)) << " and orderkey " << lineitem.orderkey << std::endl;
            }
        }
    }
    // std::cout << "Number of lineitems in results: " << results.size() << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    // std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    return std::make_pair(cost, duration.count());
}

void RandomQuery(const std::string& alg, const Tree* tree, double x, double y, int N, double rate) {
    std::vector<Orders*> result;
    int estimation_orders = 0;
    if (alg == "cl") {
        auto start_cl = std::chrono::high_resolution_clock::now();
        int cost_cl = ClassicalQuery(tree, x, y, tree->root, result);
        estimation_orders = result.size();
        auto end_cl = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_cl = end_cl - start_cl;
        meas[meas_i].t_query_cl_accum += duration_cl.count();
        meas[meas_i].c_query_cl_accum += double(cost_cl);
        meas[meas_i].t_query_cl_op_accum += duration_cl.count();
        meas[meas_i].c_query_cl_op_accum += double(cost_cl);
        std::cout << "Cost of classical query: " << cost_cl << std::endl;
        std::cout << "Time of classical query: " << duration_cl.count() << " seconds" << std::endl;
    } else if (alg == "qu") {
        auto res_qu = QuantumQuery(tree, x, y, result);
        estimation_orders = result.size();
        meas[meas_i].c_query_qu_accum += double(res_qu.first);
        meas[meas_i].t_query_qu_accum += res_qu.second;
        meas[meas_i].c_query_qu_op_accum += double(res_qu.first);
        meas[meas_i].t_query_qu_op_accum += res_qu.second;
        std::cout << "Cost of quantum query: " << res_qu.first << std::endl;
        std::cout << "Time of quantum query: " << res_qu.second << " seconds" << std::endl;
    } else if (alg == "es") {
        estimation_orders = int(rate * double(N));
    }
    std::cout << "Number of orders in result: " << estimation_orders << std::endl;
    // for (const auto& order : result) {
    //     std::cout << "Order: " << order->orderkey << " with totalprice " << order->totalprice << std::endl;
    // }

    // double rate_partsupp = 0.001;
    double rate_partsupp = rate;
    int min_availqty = 1;
    int max_availqty = 9999;
    int x_availqty = min_availqty + int(double(max_availqty - min_availqty) * Random());
    // int y_availqty = min_availqty + int(double(max_availqty - min_availqty) * Random());
    int y_availqty = x_availqty + int(double(max_availqty - min_availqty) * rate_partsupp);
    if (x_availqty > y_availqty) {
        std::swap(x_availqty, y_availqty);
    }
    std::cout << "Query range of availqty: [" << x_availqty << ", " << y_availqty << "]" << std::endl;
    int estimation_partsupp = int(rate_partsupp * double(partsupps.size()));
    std::cout << "Estimation of partsupp: " << estimation_partsupp << std::endl;

    std::pair<int, double> res1, res2;
    if (estimation_partsupp < estimation_orders) {
        std::cout << "Join with partsupp first" << std::endl;
        std::vector<LineItem> results1;
        results1.clear();
        res1 = joinWithPartsupp(x_availqty, y_availqty, partsupps, lineitems, results1);
        std::cout << "Cost of join with partsupp: " << res1.first << std::endl;
        std::cout << "Time of join with partsupp: " << res1.second << " seconds" << std::endl;
        std::cout << "Number of lineitems in results: " << results1.size() << std::endl;
        // for (const auto& lineitem : results1) {
        //     std::cout << "Lineitem: " << lineitem.orderkey << " of partkey " << lineitem.partkey << " and suppkey " << lineitem.suppkey << std::endl;
        // }
        // std::cout << "Join with orders to get the final result" << std::endl;
        std::vector<LineItem> results2;
        results2.clear();
        res2 = joinWithOrders(x, y, orders, results1, results2);
        std::cout << "Cost of join with orders: " << res2.first << std::endl;
        std::cout << "Time of join with orders: " << res2.second << " seconds" << std::endl;
        std::cout << "Number of lineitems in results: " << results2.size() << std::endl;
        // for (const auto& lineitem : results2) {
        //     std::cout << "Lineitem: " << lineitem.orderkey << " of orderkey " << lineitem.orderkey << std::endl;
        // }
    } else {
        std::cout << "Join with orders first" << std::endl;
        std::vector<LineItem> results1;
        results1.clear();
        res1 = joinWithOrders(x, y, orders, lineitems, results1);
        std::cout << "Cost of join with orders: " << res1.first << std::endl;
        std::cout << "Time of join with orders: " << res1.second << " seconds" << std::endl;
        std::cout << "Number of lineitems in results: " << results1.size() << std::endl;
        // for (const auto& lineitem : results1) {
        //     std::cout << "Lineitem: " << lineitem.orderkey << " of orderkey " << lineitem.orderkey << std::endl;
        // }
        // std::cout << "Join with partsupp to get the final result" << std::endl;
        std::vector<LineItem> results2;
        results2.clear();
        res2 = joinWithPartsupp(x_availqty, y_availqty, partsupps, results1, results2);
        std::cout << "Cost of join with partsupp: " << res2.first << std::endl;
        std::cout << "Time of join with partsupp: " << res2.second << " seconds" << std::endl;
        std::cout << "Number of lineitems in results: " << results2.size() << std::endl;
        // for (const auto& lineitem : results2) {
        //     std::cout << "Lineitem: " << lineitem.orderkey << " of orderkey " << lineitem.orderkey << std::endl;
        // }
    }

    if (alg == "cl") {
        meas[meas_i].c_query_cl_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_cl_accum += res1.second + res2.second;
        meas[meas_i].c_query_cl_pr_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_cl_pr_accum += res1.second + res2.second;
    } else if (alg == "qu") {
        meas[meas_i].c_query_qu_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_qu_accum += res1.second + res2.second;
        meas[meas_i].c_query_qu_pr_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_qu_pr_accum += res1.second + res2.second;
    } else if (alg == "es") {
        meas[meas_i].c_query_es_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_es_accum += res1.second + res2.second;
        meas[meas_i].c_query_es_pr_accum += double(res1.first + res2.first);
        meas[meas_i].t_query_es_pr_accum += res1.second + res2.second;
    }
}


// Main function to demonstrate reading all tables
int main() {
    srand(time(NULL));
    // srand(346893);

    try {
        std::ofstream ofs("plot/TPCH_join2_rand.dat");

        std::streamsize ss = std::cout.precision();
        std::cout << std::setprecision(12);

        ResetMeasurement();

        // Read data from each file and count lines
        readTable("TPCH-customer.tbl", customers);
        readTable("TPCH-lineitem.tbl", lineitems);
        readTable("TPCH-nation.tbl", nations);
        readTable("TPCH-orders.tbl", orders);
        readTable("TPCH-part.tbl", parts);
        readTable("TPCH-partsupp.tbl", partsupps);
        readTable("TPCH-region.tbl", regions);
        readTable("TPCH-supplier.tbl", suppliers);

        // // Print line counts
        // std::cout << "Customers: " << customers.size() << " lines" << std::endl;
        // computeHistogram(customers, "acctbal", &Customer::acctbal);

        // std::cout << "LineItems: " << lineitems.size() << " lines" << std::endl;
        // computeHistogram(lineitems, "quantity", &LineItem::quantity);
        // computeHistogram(lineitems, "extendedprice", &LineItem::extendedprice);
        // computeHistogram(lineitems, "shipdate", &LineItem::shipdate);
        // computeHistogram(lineitems, "commitdate", &LineItem::commitdate);
        // computeHistogram(lineitems, "receiptdate", &LineItem::receiptdate);

        // std::cout << "Nations: " << nationLines << " lines" << std::endl;

        // std::cout << "Orders: " << orders.size() << " lines" << std::endl;
        // computeHistogram(orders, "totalprice", &Orders::totalprice, 200);
        // computeHistogram(orders, "orderdate", &Orders::orderdate, 1000);

        // std::cout << "Parts: " << parts.size() << " lines" << std::endl;
        // computeHistogram(parts, "size", &Part::size);
        // computeHistogram(parts, "retailprice", &Part::retailprice);

        // std::cout << "PartSupps: " << partsupps.size() << " lines" << std::endl;
        // computeHistogram(partsupps, "availqty", &PartSupp::availqty);
        // computeHistogram(partsupps, "supplycost", &PartSupp::supplycost);

        // std::cout << "Regions: " << regions.size() << " lines" << std::endl;

        // std::cout << "Suppliers: " << suppliers.size() << " lines" << std::endl;
        // computeHistogram(suppliers, "acctbal", &Supplier::acctbal);

        std::cout << std::setprecision(10);

        // only keep the first 262144 orders
        orders.erase(orders.begin() + 262144, orders.end());

        auto tree = BuildTree(orders, orders.size());

        meas_i = 0;
        // generate a random selectivity following a distribution that favors very small values
        std::random_device rd;
        std::mt19937 gen(rd());

        // Define the exponential distribution
        double lambda = 20.0; // Rate parameter (higher values bias more towards 0)
        std::exponential_distribution<double> exp_dist(lambda);

        // Generate a random number in the range [0, 0.1]
        double selectivity = 0;
        do {
            selectivity = exp_dist(gen);
        } while (selectivity > 0.1 || selectivity < 0.00005); // Reject values outside the desired range

        std::cout << "Selectivity: " << selectivity << std::endl;

        for (int j = 0; j < REPEAT; j++) {
            // generate a random range [x, y] from 853 to 493724
            std::cout << "Running iteration " << (j + 1) << " of repeat " << REPEAT << std::endl << std::endl;

            int min = 853;
            int max = 493724;
            int x = min + int(double(max - min) * Random());
            int y = x + int(double(max - min) * selectivity);
            // // x,y = [475700, 475749]
            // x = 475700;
            // y = 475749;
            std::cout << "Random range: [" << x << ", " << y << "]" << std::endl;

            std::cout << "Running classical query" << std::endl;
            RandomQuery("cl", tree, x, y, orders.size(), selectivity);
            std::cout << std::endl;
            std::cout << "Running quantum query" << std::endl;
            RandomQuery("qu", tree, x, y, orders.size(), selectivity);
            std::cout << std::endl;
            std::cout << "Running estimation query" << std::endl;
            RandomQuery("es", tree, x, y, orders.size(), selectivity);
            std::cout << std::endl;

            // if (j > 8) {
            //     break;
            // }
        }

        ComputeMeasurement(meas_i);
        PrintMeasurements(meas_i + 1);
        std::cout << std::endl;

        ofs << std::setprecision(8);
        ofs << selectivity << "\t";
        meas[meas_i].write(ofs);
        ofs << std::endl;

        ofs.close();

        delete tree;

        std::cout << "Done" << std::endl;
        std::cout.flush();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}