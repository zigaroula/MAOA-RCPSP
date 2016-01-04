#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int LB3 = 0;
int h = 0;
int r = 0;

int m; // number of machines
vector<int> b; // capacities of machines

int n; // number of tasks
vector<vector<int> > ress; // ressources required
vector<vector<int> > adjList; // initial adjacence list
vector<vector<int> > adjMatrix; // initial adjacence matrix
vector<int> d; // length of tasks

vector<vector<int> > G; // adjacence list of new graph
vector<int> Sh;
vector<int> L;

void step1();
void step2();
void step3();

void readValues() { // in
    cin >> m;
    for (int i = 0 ; i < m ; i++) {
        int tmp;
        cin >> tmp;
        b.push_back(tmp);
    }

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
    for (int i = 0 ; i < n ; i++) {
        int degree;
        cin >> degree;
        vector<int> tmpvec;
        for (int j = 0 ; j < degree ; j++) {
            int tmp;
            cin >> tmp;
            tmpvec.push_back(tmp-1);
            adjMatrix[i][tmp-1] = 1;
        }
        adjList.push_back(tmpvec);
    }
}

void floydWarshall() {
    for (int i = 0 ; i < n ; i++) {
        for (int j = 0 ; j < n ; j++) {
            for (int k = 0 ; k < n ; k++) {
                adjMatrix[i][j] = min(adjMatrix[i][j], adjMatrix[i][k] + adjMatrix[k][j]);
            }
        }
    }
}

bool path(int i, int j) { // is there a path from i to j
    return adjMatrix[i][j] != 1000000;
}

void computeGraph() { // compute new graph
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

void computeList() { // compute L
    // TODO
    L.push_back(2);
    L.push_back(5);
    L.push_back(9);
    L.push_back(3);
    L.push_back(6);
    L.push_back(8);
    L.push_back(1);
    L.push_back(4);
    L.push_back(7);
}

bool intersectionIsNull(int i) { // check if intersection is null
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
    r = h;
    step2();
}

void step2() {
    int is = -1;
    int s = 0;
    for (int i = r ; i < L.size() ; i++) {
        if (intersectionIsNull(L[i])) {
            is = L[i];
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
    h++;
    if (h <= n-2){
        printTrace(deltah);
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
