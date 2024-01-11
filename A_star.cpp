#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

vector<vector<int>> WczytajGrid() {
    ifstream plik("grid.txt");
    vector<vector<int>> mapa;

    string temp;

    for (int i = 0; i < 20; i++) {
        mapa.push_back(vector<int>());
        getline(plik, temp);
        for (int j = 0; j < temp.size(); j++) {
            if (temp[j] != ' ') {
                mapa[i].push_back(temp[j] - 48);
            }
        }
    }

    plik.close();
    return mapa;
}

void Rysuj(vector<vector<int>> mapa) {
    for (int i = 0; i < mapa.size(); i++) {
        for (int j = 0; j < mapa[i].size(); j++) {
            cout << mapa[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

void Rysuj(vector<vector<int>> mapa, vector<pair<int, int>> droga) {
    for (int i = 0; i < droga.size(); i++) {
        mapa[droga[i].first][droga[i].second] = 1;
    }

    for (int i = 0; i < mapa.size(); i++) {
        for (int j = 0; j < mapa[i].size(); j++) {
            if (mapa[i][j] == 5) cout << "\033[31m" << mapa[i][j];
            else if (mapa[i][j] == 1) cout << "\033[33m" << mapa[i][j];
            else cout << "\033[0m" << mapa[i][j];
        }
        cout << "\033[0m" << endl;
    }

    cout << endl;
}

class Node {
public:
    Node(int x, int y, int koszt, int odleglosc, Node* rodzic) {
        this->x = x;
        this->y = y;
        this->koszt = koszt;
        this->odleglosc = odleglosc;
        this->rodzic = rodzic;
    }

    int x = 0;
    int y = 0;
    int koszt = 0;
    int odleglosc = 0;

    Node* rodzic = NULL;

    int przelicz() {
        return koszt + odleglosc;
    }
};

struct PorownajNode {
    bool operator()(Node* a, Node* b) {
        return a->przelicz() > b->przelicz();
    }
};

vector<pair<int, int>> astar(const vector<vector<int>>& grid, pair<int, int> start, pair<int, int> cel) {
    if (grid[start.first][start.second] == 5) {
        return {};
    }

    if (grid[cel.first][cel.second] == 5) {
        return {};
    }

    int wiersze = grid.size();
    int kolumny = grid[0].size();

    priority_queue<Node*, vector<Node*>, PorownajNode> openSet;

    Node* startNode = new Node(start.first, start.second, 0, 0, nullptr);
    openSet.push(startNode);

    vector<vector<bool>> odwiedzone(wiersze, vector<bool>(kolumny, false));
    odwiedzone[start.first][start.second] = true;

    while (!openSet.empty()) {
        Node* odecnyNode = openSet.top();
        openSet.pop();

        if (odecnyNode->x == cel.first && odecnyNode->y == cel.second) {
            vector<pair<int, int>> sciezka;
            while (odecnyNode != nullptr) {
                sciezka.push_back({ odecnyNode->x, odecnyNode->y });
                odecnyNode = odecnyNode->rodzic;
            }
            reverse(sciezka.begin(), sciezka.end());

            while (!openSet.empty()) {
                delete openSet.top();
                openSet.pop();
            }

            return sciezka;
        }

        vector<pair<int, int>> sasiedzi = {
            {odecnyNode->x - 1, odecnyNode->y},
            {odecnyNode->x + 1, odecnyNode->y},
            {odecnyNode->x, odecnyNode->y - 1},
            {odecnyNode->x, odecnyNode->y + 1}
        };

        for (const auto& sasiad : sasiedzi) {
            int x = sasiad.first;
            int y = sasiad.second;

            if (x >= 0 && x < wiersze && y >= 0 && y < kolumny && grid[x][y] != 5 && !odwiedzone[x][y]) {
                int koszt = odecnyNode->koszt + 1;
                int heurystyka = abs(x - cel.first) + abs(y - cel.second);
                Node* sasiadNode = new Node(x, y, koszt, heurystyka, odecnyNode);

                openSet.push(sasiadNode);

                odwiedzone[x][y] = true;
            }
        }
    }

    while (!openSet.empty()) {
        delete openSet.top();
        openSet.pop();
    }

    // Brak ścieżki do celu
    return {};
}

int main()
{
    auto mapa = WczytajGrid();
    Rysuj(mapa);

    pair<int, int> start;
    start.first = 0;
    start.second = 0;

    pair<int, int> cel;
    cel.first = 19;
    cel.second = 19;

    auto wynik = astar(mapa, start, cel);
    Rysuj(mapa, wynik);

    return 0;
}
