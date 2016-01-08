#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int LB3 = 0;
int h = 1;
int r = 0;

int m; // number of machines
vector<int> b; // capacities of machines

int n; // number of tasks
vector<vector<int> > ress; // ressources required
vector<vector<int> > adjList; // initial adjacence list
vector<vector<int> > adjMatrix; // initial adjacence matrix
vector<vector<int> > adjMatrixW; // initial weighted adjacence matrix
vector<vector<int> > next; // for longest path (next[i][j] is k stated that i->k->j)
vector<int> d; // length of tasks

vector<vector<int> > G; // adjacence list of new graph
vector<int> Sh; // list of vertices already used
vector<int> L; // circular list

void step1();
void step2();
void step3();

void readValues() { // initialization and reading of text file
    // machines properties
    cin >> m;
    for (int i = 0 ; i < m ; i++) {
        int tmp;
        cin >> tmp;
        b.push_back(tmp);
    }

    // tasks properties
    cin >> n;
    for (int i = 0 ; i < n ; i++) {
        int tmp;
        cin >> tmp;
        d.push_back(tmp);
        vector<int> tmpvec;
        for (int j = 0 ; j < m ; j++) {
            int tmp2;
            cin >> tmp2;
            tmpvec.push_back(tmp2);
        }
        ress.push_back(tmpvec);
    }
    adjMatrix = vector<vector<int> >(n, vector<int>(n, 1000000));
    adjMatrixW = vector<vector<int> >(n, vector<int>(n, 1000000));
    for (int i = 0 ; i < n ; i++) {
        int degree;
        cin >> degree;
        vector<int> tmpvec;
        for (int j = 0 ; j < degree ; j++) {
            int tmp;
            cin >> tmp;
            tmpvec.push_back(tmp-1);
            adjMatrix[i][tmp-1] = 1;
            adjMatrixW[i][tmp-1] = - d[tmp-1];
        }
        adjList.push_back(tmpvec);
    }

    // init
    next = vector<vector<int> >(n, vector<int>(n, -1));
}

void floydWarshall() { // computes if a path exists between i and j and the longest path between i and j
    for (int i = 0 ; i < n ; i++) {
        for (int j = 0 ; j < n ; j++) {
            for (int k = 0 ; k < n ; k++) {
                adjMatrix[i][j] = min(adjMatrix[i][j], adjMatrix[i][k] + adjMatrix[k][j]);
                if (adjMatrixW[i][j] > adjMatrixW[i][k] + adjMatrixW[k][j]) {
                    adjMatrixW[i][j] = adjMatrixW[i][k] + adjMatrixW[k][j];
                    next[i][j] = k;
                }
            }
        }
    }
}

vector<int> getPath(int i, int j) { // returns longest path between i and j

    // if there is no path or there is no intermediate vertex, returns nothing
    if (adjMatrixW[i][j] > 0) return vector<int>();
    int intermediate = next[i][j];
    if (intermediate < 0) return vector<int>();

    // computes left and right of intermediate
    vector<int> path1, path2;
    path1 = getPath(i, intermediate);
    path2 = getPath(intermediate, j);

    vector<int> result;
    for (int i = 0 ; i < path1.size() ; i++) {
        result.push_back(path1[i]);
    }
    result.push_back(intermediate);
    for (int i = 0 ; i < path2.size() ; i++) {
        result.push_back(path2[i]);
    }
    return result;
}

bool path(int i, int j) { // is there a path from i to j
    return adjMatrix[i][j] != 1000000;
}

void computeGraph() { // computes new graph G' (algorithm from the article)
    G = vector<vector<int> >(n);
    for (int i = 1 ; i < n-1 ; i++) {
        for (int j = 1 ; j < n-1 ; j++) {
            bool add = false;
            if ( (i < j && !path(i, j)) || (j < i && !path(j, i)) ) {
                int cpt = 0;
                for (int k = 0 ; k < m ; k++) {
                    if (ress[i][k] + ress[j][k] <= b[k]) {
                        cpt++;
                    }
                }
                if (cpt == m) {
                    add = true;
                }
            }
            if (add) {
                G[i].push_back(j);
            }
        }
    }
}

bool compare(const vector<int> &a, const vector<int> &b) { // function used to sort vertices by adjacence degrees
    return a[1] > b[1];
}

void computeList() { // computes L
    // gets the first elements from the longest path from O to n-1
    vector<int> pi;
    pi = getPath(0, n-1);
    for (int i = 0 ; i < pi.size() ; i++) {
        L.push_back(pi[i]);
    }

    // gets the other elements by ordering for nondecreasing values of their degree in G'
    vector<vector<int> > degrees;
    for (int i = 0 ; i < G.size() ; i++) {
        vector<int> tmp;
        tmp.push_back(i);
        tmp.push_back(G[i].size());
        degrees.push_back(tmp);
    }
    sort(degrees.begin(), degrees.end(), compare);
    for (int i = 0 ; i < degrees.size() ; i++) {
        if (find(L.begin(), L.end(), degrees[i][0]) == L.end() && degrees[i][0] != 0 && degrees[i][0] != n-1) {
            L.push_back(degrees[i][0]);
        }
    }
}

bool intersectionIsNull(int i) { // checks if intersection is null
    bool res = true;
    for (int j = 0 ; j < G[i].size() ; j++) {
        if (find(Sh.begin(), Sh.end(), G[i][j]) != Sh.end()) {
            res = false;
        }
    }
    return res;
}

void printTrace(int deltah) {
    cout << "h = " << h << " | ";
    for (int i = 0 ; i < Sh.size() ; i++) {
        cout << Sh[i] + 1 << " ";
    }
    cout << "| " << deltah << endl;
}

void step1() {
    Sh.clear();
    r = h - 1;
    step2();
}

void step2() {
    int is = -1;
    int s = 0;
    for (int i = r ; i < h - 1 + L.size() ; i++) {
        int i2 = i % L.size();
        if (intersectionIsNull(L[i2])) {
            is = L[i2];
            s = i;
            break;
        }
    }
    if (is == -1) {
        step3();
    } else {
        r = s + 1;
        Sh.push_back(is);
        step2();
    }
}

void step3() {
    int deltah = 0;
    for (int i = 0 ; i < Sh.size() ; i++) {
        deltah += d[Sh[i]];
    }
    if (deltah > LB3) {
        LB3 = deltah;
    }
    printTrace(deltah);
    h++;
    if (h <= n-2){
        step1();
    }
}

int main(void) {
    readValues();
    floydWarshall();
    computeGraph();
    computeList();
    step1();
    cout << LB3 << endl;
    return 0;
}
