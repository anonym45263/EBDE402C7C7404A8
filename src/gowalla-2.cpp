#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;

void quicksort(pair<double, double> a[], int l, int r){
    pair<double, double> m = a[(l + r) / 2];
    int i = l;
    int j = r;
    while (i < j){
        while (a[i] < m)
            i++;
        while (a[j] > m)
            j--;
        if (i <= j){
            swap(a[i], a[j]);
            i++;
            j--;
        }
    }
    if (i < r) quicksort(a, i, r);
    if (j > l) quicksort(a, l, j);
}


pair<double, double> input[3000000], tmp[3000000];
double tmp1[3000000];
int N = 2097152;
int cnt = 0;
int qcnt = 0;
const int B = 64;
ofstream out;

struct Node{
    double l[B], r[B], y[B];
    int c[B];
    int weight;
    int height;
};

struct Tree{
    Node *node;
    double *data;
    int root;
    int N;
    bool X;
    Tree **nextTree;
    Tree(int size, bool flag){
        node = new Node[size];
        data = new double[size];
        X = flag;
        if (X) {
            nextTree = new Tree *[size];
            for (int i = 0; i < size; i++)
                nextTree[i] = nullptr;
        }
        N = size;
    };
    ~Tree(){
        delete node;
        delete data;
        if (X) {
            for (int i = 0; i < N; i++) {
                if (nextTree[i] != nullptr) {
                    delete nextTree[i];
                }
            }
            delete nextTree;
        }
    };
};

vector<pair<Tree*, int>> V;

void ReadInput(const string& file){
    ifstream in;
    in.open(file);

    string s;
    double x, y;
    for (int i = 0; i < N; i++){
        in >> s >> s >> x >> y >> s;
        input[i] = make_pair(x, y);
    }
    in.close();
}

int Random(int x){
    return rand() % x;
}

void CompleteTree(Tree *tree){
    int N = tree->N;
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
}

Tree *BuildTree(){
    Tree *tree = new Tree(N, true);
    for (int i = 0; i < N; i++)
        tmp[i] = input[i];
    quicksort(tmp, 0, N - 1);
    for (int i = 0; i < N; i++)
        tree->data[i] = tmp[i].first;
    for (int i = 0; i < N / B; i++){
        tree->node[i].weight = B;
        tree->node[i].height = 1;
        tree->nextTree[i] = new Tree(B, false);
        for (int j = 0; j < B; j++) {
            tree->node[i].c[j] = -1;
            tree->node[i].l[j] = tree->data[i * B + j];
            tree->node[i].r[j] = tree->data[i * B + j];
            tree->node[i].y[j] = tmp[i * B + j].second;
            tree->nextTree[i]->data[j] = tmp[i * B + j].second;
        }
        CompleteTree(tree->nextTree[i]);
    }
    int l = 0;
    int cnt = N / B;
    while (cnt > l + 1){
        int r = cnt;
        for (int i = 0; i < (r - l) / B; i++){
            tree->node[cnt].weight = tree->node[l + i * B].weight * B;
            tree->node[cnt].height = tree->node[l + i * B].height + 1;
            tree->nextTree[cnt] = new Tree(tree->node[cnt].weight, false);
            for (int j = 0; j < B; j++){
                tree->node[cnt].c[j] = l + i * B + j;
                tree->node[cnt].l[j] = tree->node[l + i * B + j].l[0];
                tree->node[cnt].r[j] = tree->node[l + i * B + j].r[B - 1];
                for (int k = 0; k < tree->node[l + i * B].weight; k++)
                    tree->nextTree[cnt]->data[tree->node[l + i * B].weight * j + k] = tree->nextTree[l + i * B + j]->data[k];
            }
            CompleteTree(tree->nextTree[cnt]);
            cnt++;
        }
        if (r - l < B){
            tree->node[cnt].weight = tree->node[l].weight * (r - l);
            tree->node[cnt].height = tree->node[l].height + 1;
            tree->nextTree[cnt] = new Tree(tree->node[cnt].weight, false);
            for (int j = l; j < r; j++){
                tree->node[cnt].c[j - l] = j;
                tree->node[cnt].l[j - l] = tree->node[j].l[0];
                tree->node[cnt].r[j - l] = tree->node[j].r[B - 1];
                for (int k = 0; k < tree->node[l].weight; k++)
                    tree->nextTree[cnt]->data[tree->node[l].weight * (j - l) + k] = tree->nextTree[j]->data[k];
            }
            for (int j = r - l; j < B; j++)
                tree->node[cnt].l[j] = tree->node[cnt].r[j] = -1e9;
            CompleteTree(tree->nextTree[cnt]);
            cnt++;
        }
        l = r;
    }
    tree->root = cnt - 1;
    return tree;
}

int ClassicalQueryY(double x, double y, Tree *tree, int u){
    cnt++;
    int ans = 0;
    for (int i = 0; i < B; i++)
        if ((tree->node[u].l[i] <= y) && (tree->node[u].r[i] >= x)){
            if (tree->node[u].c[i] == -1)
                ans++;
            else
                ans += ClassicalQueryY(x, y, tree, tree->node[u].c[i]);
        }
    return ans;
}

int ClassicalQuery(double x1, double y1, double x2, double y2, Tree *tree, int u){
    cnt++;
    qcnt++;
    int ans = 0;
    if (tree->node[u].c[0] == -1){
        bool in = false;
        for (int i = 0; i < B; i++)
            if ((tree->node[u].l[i] >= x1) && (tree->node[u].r[i] <= y1) && (tree->node[u].y[i] >= x2) && (tree->node[u].y[i] <= y2)){
                ans++;
                in = true;
            }
        if (in)
            V.push_back(make_pair(tree->nextTree[u], tree->nextTree[u]->root));
    }
    else {
        for (int i = 0; i < B; i++) {
            if ((tree->node[u].l[i] >= x1) && (tree->node[u].r[i] <= y1)) {
                ans += ClassicalQueryY(x2, y2, tree->nextTree[tree->node[u].c[i]], tree->nextTree[tree->node[u].c[i]]->root);
                V.push_back(make_pair(tree->nextTree[tree->node[u].c[i]], tree->nextTree[tree->node[u].c[i]]->root));
            } else if ((tree->node[u].l[i] <= y1) && (tree->node[u].r[i] >= x1)) {
                ans += ClassicalQuery(x1, y1, x2, y2, tree, tree->node[u].c[i]);
            }
        }
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

void QuantumQuery(double x, double y, vector<pair<Tree*, int>> V, int k){
    while (!V.empty()){
        qcnt++;
        int pos = 0;
        for (int i = 1; i < V.size(); i++)
            if (V[i].first->node[V[i].second].weight > V[pos].first->node[V[pos].second].weight)
                pos = i;
        int u =  V[pos].second;
        Tree *tree = V[pos].first;
        V.erase(V.begin() + pos);
        for (int i = 0; i < B; i++) {
            if ((tree->node[u].l[i] >= x) && (tree->node[u].l[i] <= y)) {
                int weight = tree->node[u].weight;
                for (int j = 0; j < V.size(); j++)
                    weight += V[j].first->node[V[j].second].weight;
                qcnt += QuantumSim(tree->node[u].height + V.size(), weight, k);
                return;
            } else if ((tree->node[u].l[i] <= y) && (tree->node[u].r[i] >= x) && (tree->node[u].c[i] != -1))
                V.push_back(make_pair(tree, tree->node[u].c[i]));
        }
    }
}

int cntans = 0;

void RandomQuery(int I, Tree * tree, double rate){
    for (int i = 0; i < N; i++)
        tmp[i] = input[i];
    quicksort(tmp, 0, N - 1);
    int n = N * rate;
    for (int i = 0; i < I; i++){
        int t = Random(N - n - 1);
        double x1 = tmp[t].first;
        double y1 = tmp[t + n].first;
        double x2 = 1e9;
        double y2 = -1e9;
        for (int j = t; j <= t + n; j++){
            x2 = min(x2, tmp[j].second);
            y2 = max(y2, tmp[j].second);
        }
        V.clear();
        int ans = ClassicalQuery(x1, y1, x2, y2, tree, tree->root);
        cntans += ans;
        QuantumQuery(x2, y2, V, ans);
    }
}

int main() {
    srand(time(NULL));

    // ReadInput("loc-gowalla_totalCheckins.txt");
    ReadInput("gowalla_totalCheckins.txt");
    out.open("Gowalla-2-" + to_string(B) + ".txt");
    for (double O = 0.05; O <= 0.05; O += 0.01){
        cnt = qcnt = 0;
        cntans = 0;
        Tree *tree = BuildTree();
        RandomQuery(10000, tree, O);
        cout << O << "\t" << cnt / 10000.0 << "\t" << qcnt / 10000.0 << endl;
        out << O << "\t" << cnt / 10000.0 << "\t" << qcnt / 10000.0 << endl;
        delete tree;
    }

    out.close();

    return 0;
}
