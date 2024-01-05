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

struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->przelicz() > b->przelicz();
    }
};

vector<pair<int, int>> astar(const vector<vector<int>>& grid, pair<int, int> start, pair<int, int> goal) {
    if (grid[start.first][start.second] == 5) {
        return {};
    }

    if (grid[goal.first][goal.second] == 5) {
        return {};
    }

    // Wymiary planszy
    int rows = grid.size();
    int cols = grid[0].size();

    // Kolejka priorytetowa do przechowywania węzłów
    priority_queue<Node*, vector<Node*>, CompareNodes> openSet;

    // Tworzymy węzeł startowy
    Node* startNode = new Node(start.first, start.second, 0, 0, nullptr);
    openSet.push(startNode);

    // Tablica odwiedzonych węzłów
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    visited[start.first][start.second] = true;

    // Szukamy ścieżki
    while (!openSet.empty()) {
        // Pobieramy węzeł z najniższym kosztem
        Node* currentNode = openSet.top();
        openSet.pop();

        // Sprawdzamy, czy osiągnęliśmy cel
        if (currentNode->x == goal.first && currentNode->y == goal.second) {
            // Odtwarzamy ścieżkę
            vector<pair<int, int>> path;
            while (currentNode != nullptr) {
                path.push_back({ currentNode->x, currentNode->y });
                currentNode = currentNode->rodzic;
            }
            reverse(path.begin(), path.end());

            // Zwolniamy pamięć z zajętą przez węzły
            while (!openSet.empty()) {
                delete openSet.top();
                openSet.pop();
            }

            return path;
        }

        // Przeglądamy sąsiadów aktualnego węzła
        vector<pair<int, int>> neighbors = {
            {currentNode->x - 1, currentNode->y},
            {currentNode->x + 1, currentNode->y},
            {currentNode->x, currentNode->y - 1},
            {currentNode->x, currentNode->y + 1}
        };

        for (const auto& neighbor : neighbors) {
            int x = neighbor.first;
            int y = neighbor.second;

            // Sprawdzamy, czy sąsiad jest w granicach planszy i nie jest przeszkodą
            if (x >= 0 && x < rows && y >= 0 && y < cols && grid[x][y] != 5 && !visited[x][y]) {
                // Tworzymy węzeł dla sąsiada
                int cost = currentNode->koszt + 1;  // Założenie: koszt do każdego sąsiada jest równy 1
                int heuristic = abs(x - goal.first) + abs(y - goal.second);  // Heurystyka (np. odległość Manhattan)
                Node* neighborNode = new Node(x, y, cost, heuristic, currentNode);

                // Dodajemy sąsiada do kolejki priorytetowej
                openSet.push(neighborNode);

                // Oznaczamy sąsiada jako odwiedzonego
                visited[x][y] = true;
            }
        }
    }

    // Zwolniamy pamięć z zajętą przez węzły
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

    pair<int, int> goal;
    goal.first = 19;
    goal.second = 19;

    auto wynik = astar(mapa, start, goal);
    Rysuj(mapa, wynik);
    int madzia = 12;

    return 0;
}
