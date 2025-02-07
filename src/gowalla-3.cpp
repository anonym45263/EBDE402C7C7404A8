#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;


double input[5000000];
int N = 2097152;
int cnt = 0;
int qcnt = 0;
const int B = 16;

struct Node{
    double l[B], r[B];
    int c[B];
    int weight;
    int height;
};

struct Tree{
    Node *node;
    double *data;
    int root;
    Tree(int size){
        node = new Node[size];
        data = new double[size];
    };
    ~Tree(){
        delete node;
        delete data;
    };
};



ofstream out;

void ReadInput(const string& file){
    ifstream in;
    in.open(file);

    tm time{};
    string S, s;
    for (int i = 0; i < N; i++){
        in >> s >> S >> s >> s >> s;
        memset(&time, 0, sizeof(time));
        time.tm_year = atoi(S.c_str());
        time.tm_mon = atoi(S.c_str() + 5);
        time.tm_mday = atoi(S.c_str() + 8);
        time.tm_hour = atoi(S.c_str() + 11);
        time.tm_min = atoi(S.c_str() + 14);
        time.tm_sec = atoi(S.c_str() + 17);
        input[i] = mktime(&time);
    }

    in.close();
}

int Random(int x){
    return rand() % x;
}

Tree *BuildTree(){
    Tree *tree = new Tree(N);
    memcpy(tree->data, input, sizeof(double) * N);
    sort(tree->data, tree->data + N);
    for (int i = 0; i < N / B; i++){
        tree->node[i].weight = B;
        tree->node[i].height = 1;
        for (int j = 0; j < B; j++) {
            tree->node[i].c[j] = -1;
            tree->node[i].l[j] = tree->data[i * B + j];
            tree->node[i].r[j] = tree->data[i * B + j];
        }
    }
    int l = 0;
    int cnt = N / B;
    while (cnt > l + 1){
        int r = cnt;
        for (int i = 0; i < (r - l) / B; i++){
            tree->node[cnt].weight = tree->node[l + i * B].weight * B;
            tree->node[cnt].height = tree->node[l + i * B].height + 1;
            for (int j = 0; j < B; j++){
                tree->node[cnt].c[j] = l + i * B + j;
                tree->node[cnt].l[j] = tree->node[l + i * B + j].l[0];
                tree->node[cnt].r[j] = tree->node[l + i * B + j].r[B - 1];
            }
            cnt++;
        }
        if (r - l < B){
            tree->node[cnt].weight = tree->node[l].weight * (r - l);
            tree->node[cnt].height = tree->node[l].height + 1;
            for (int j = l; j < r; j++){
                tree->node[cnt].c[j - l] = j;
                tree->node[cnt].l[j - l] = tree->node[j].l[0];
                tree->node[cnt].r[j - l] = tree->node[j].r[B - 1];
            }
            for (int j = r - l; j < B; j++)
                tree->node[cnt].l[j] = tree->node[cnt].r[j] = -1e9;
            cnt++;
        }
        l = r;
    }
    tree->root = cnt - 1;
    return tree;
}

int ClassicalQuery(double x, double y, Tree *tree, int u){
    cnt++;
    int ans = 0;
    for (int i = 0; i < B; i++)
        if ((tree->node[u].l[i] <= y) && (tree->node[u].r[i] >= x)){
            if (tree->node[u].c[i] == -1)
                ans++;
            else
                ans += ClassicalQuery(x, y, tree, tree->node[u].c[i]);
        }
    return ans;
}

int QuantumSim(int height, int weight, int k){
    int tmp = height;
    int x = Random(weight);
    while (x > k){
        x = Random(weight);
        tmp += height;
    }
    return tmp;
}

void QuantumQuery(double x, double y, Tree *tree, vector<int> V, int k){
    vector<int> U;
    int height = tree->node[V[0]].height;
    int weight = 0;
    for (int i = 0; i < V.size(); i++)
        weight += tree->node[V[i]].weight;
    for (int j = 0; j < V.size(); j++) {
        for (int i = 0; i < B; i++) {
            qcnt++;
            if ((tree->node[V[j]].l[i] >= x) && (tree->node[V[j]].l[i] <= y)) {
                qcnt += QuantumSim(height, weight, k);
                return;
            } else if ((tree->node[V[j]].l[i] <= y) && (tree->node[V[j]].r[i] >= x) && (tree->node[V[j]].c[i] != -1))
                U.push_back(tree->node[V[j]].c[i]);
        }
    }
    if (U.size())
        QuantumQuery(x, y, tree, U, k);
}

void RandomQuery(int I, Tree * tree, double rate){
    sort(input, input + N);
    double l = input[N / 10];
    double r = input[N / 10 * 9];
    int n = rate * N;
    for (int i = 0; i < I; i++){
        int t = Random(N - n - 1);
        double x = input[t];
        double y = input[t + n];
        int ans = ClassicalQuery(x, y, tree, tree->root);
        vector<int> V;
        V.push_back(tree->root);
        QuantumQuery(x, y, tree, V, ans);
    }
}

int main() {
    srand(time(NULL));

    // ReadInput("loc-gowalla_totalCheckins.txt");
    ReadInput("gowalla_totalCheckins.txt");
    out.open("Gowalla-1-" + to_string(B) + ".txt");
    for (int O = 4096; O <= 2097152; O *= 2){
        N = O;
        cnt = qcnt = 0;
        Tree *tree = BuildTree();
        RandomQuery(10000, tree, 0.05);
        cout << O << "\t" << cnt / 10000.0 << "\t" << qcnt / 10000.0 * log(O) / log(B)  << endl;
        out << O << "\t" << cnt / 10000.0 << "\t" << qcnt / 10000.0 * log(O) / log(B)  << endl;
        delete tree;
    }

    out.close();

    return 0;
}
