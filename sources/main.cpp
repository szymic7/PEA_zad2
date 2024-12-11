#include <iostream>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include "../headers/Greedy.h"
#include "../headers/TabuSearch.h"
#include "../headers/SimulatedAnnealing.h"

using namespace std;

//-------------------------------------------------------------------------------------------------

void showMenu()
{
    int choice = 0, n = 0, choiceNeighborhood = 0, choiceAlfa = 0;
    string filename;
    bool quit = false;

    // Macierz kosztow
    int **costMatrix = nullptr;

    // Obiekty klas reprezentujacych algorytmy
    Greedy greedy;
    TabuSearch tabuSearch;
    SimulatedAnnealing simulatedAnnealing;

    // Kryterium stopu dla algorytmow
    int stopCriterion = 0;

    // Zmienna do sledzenia, ktory z algorytmow zostal ostatnio wykonany
    int recentAlgorithm = 0;

    // Ostatnie znalezione rozwiazanie
    vector<int> result;
    result.clear();

    // Inicjalizacja generatora liczb losowych
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);

    // Pomiar czasu
    chrono::high_resolution_clock::time_point startTime, endTime;
    chrono::duration<double, std::milli> time(0);


    do {
        cout << endl << "Wybierz operacje: " << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "1) Wczytaj dane z pliku" << endl;
        cout << "2) Ustaw kryterium stopu dla algorytmow" << endl;
        cout << "3) Wyznacz rozwiazanie metoda zachlanna" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "4) Wybierz definicje sasiedztwa dla algorytmu Tabu-search" << endl;
        cout << "5) Wykonaj algorytm Tabu-search" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "6) Ustaw wspolczynnik zmiany temperatury dla algorytmu SW" << endl;
        cout << "7) Wykonaj algorytm Symulowanego Wyzarzania" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "8) Zapisz sciezke rozwiazania do pliku .txt" << endl;
        cout << "9) Wczytaj sciezke z pliku .txt i oblicz koszt" << endl;
        cout << "10) Zakoncz" << endl;
        cout << "----------------------------------------------------------" << endl;
        cout << "Wybor:";
        cin >> choice;

        switch(choice) {
            case 1: // wczytaj tablice z pliku XML
            {
                cout << endl << "Podaj nazwe pliku: " << endl;
                cin >> filename;
                ifstream file(filename);

                if(!file.good()) {
                    cout << endl << "W projekcie nie ma pliku o takiej nazwie." << endl;
                    file.close();
                    break;
                }

                // Usuniecie poprzedniej macierzy przed zapisaniem nowego rozmiaru n
                if(costMatrix != nullptr) {
                    for (int i = 0; i < n; ++i) {
                        delete[] costMatrix[i];
                    }
                    delete[] costMatrix;
                }
                costMatrix = nullptr;

                std::string line;
                n = 0;

                while (std::getline(file, line)) {
                    if (line.find("DIMENSION") != std::string::npos) {
                        // Zczytanie rozmiaru macierzy z linijki z "DIMENSION"
                        n = std::stoi(line.substr(line.find(":") + 1));
                    }
                    if (line.find("EDGE_WEIGHT_SECTION") != std::string::npos) {
                        break;
                    }
                }

                if (n == 0) {
                    std::cerr << "Error: Matrix size not found in file" << std::endl;
                    return;
                }

                // Allocate dynamic 2D array
                costMatrix = new int*[n];
                for (int i = 0; i < n; ++i) {
                    costMatrix[i] = new int[n];
                }

                // Read the matrix values
                int row = 0, col = 0;
                while (std::getline(file, line) && row < n) {
                    std::istringstream stream(line);
                    int value;
                    while (stream >> value) {
                        costMatrix[row][col++] = value;
                        if (col == n) {
                            col = 0;
                            row++;
                        }
                    }
                }

                file.close();

                // Wyswietlenie macierzy
                cout << endl << "Macierz kosztow:" << endl;
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        int number = costMatrix[i][j];
                        cout << number << " ";
                        if (number < 100) cout << " ";
                        if (number > 0 && number < 10) cout << " ";
                    }
                    cout << endl;
                }

                break;
            }

            case 2: // ustaw kryterium stopu
            {
                cout << endl << "Podaj wartosc kryterium stopu dla algorytmow (w pelnych sekundach): ";
                cin >> stopCriterion;

                if(stopCriterion > 0) {
                    tabuSearch.setTimeLimit(stopCriterion);
                    simulatedAnnealing.setTimeLimit(stopCriterion);
                    cout << endl << "Kryterium stopu zostalo ustawione dla algorytmow TS i SW." << endl;
                } else {
                    cout << endl << "Bledna wartosc. Kryterium stopu nie zostalo ustawione." << endl;
                }

                break;
            }

            case 3: // rozwiazanie metoda zachlanna
            {
                if (costMatrix != nullptr) {

                    greedy.setN(n);
                    greedy.setCostMatrix(costMatrix);

                    greedy.greedyAlgorithm();
                    cout << "Wynik dzialania algorytmu zachlannego: " << greedy.getResult() << endl;
                    cout << "Najlepsza sciezka: ";
                    greedy.printResultVertices();
                    cout << endl;

                } else {
                    cout << endl << "Nie wczytano macierzy kosztow. Wczytaj macierz, aby wykonac algorytm." << endl;
                }
                break;
            }

            case 4: // wybor definicji sasiedztwa dla TS
            {
                do {

                    cout << endl << "Wybierz definicje sasiedztwa:" << endl;
                    cout << "----------------------------------------------------------------------------------" << endl;
                    cout << "1) Insert" << endl;
                    cout << "2) Swap" << endl;
                    cout << "3) Invert" << endl;
                    cout << "Wybor:";
                    cin >> choiceNeighborhood;

                    if(choiceNeighborhood < 1 || choiceNeighborhood > 3) {
                        cout << endl << "Nieprawidlowy numer wybranej opcji. Sprobuj ponownie." << endl;
                        choiceNeighborhood = 0;
                    }

                } while (choiceNeighborhood == 0);

                tabuSearch.setNeighborhoodDef(choiceNeighborhood);

                break;
            }

            case 5: // Algorytm Tabu-search
            {
                if (costMatrix != nullptr) {

                    tabuSearch.setN(n);
                    tabuSearch.setCostMatrix(costMatrix);

                    tabuSearch.algorithm();
                    result = tabuSearch.getResultVector();
                    cout << "Koszt znalezionego rozwiazania: " << tabuSearch.calculatePathCost(result) << endl;
                    cout << "Znalezione rozwiazanie: ";
                    for(const int& vertice : result)
                        cout << vertice << " ";

                    //cout << "Wynik: " << tabuSearch.getResult() << endl;
                    //cout << "Najlepsza sciezka: ";
                    //tabuSearch.printResultVertices();
                    cout << endl;

                } else {
                    cout << endl << "Nie wczytano macierzy kosztow. Wczytaj macierz, aby wykonac algorytm." << endl;
                }
                break;
            }

            case 6: // ustaw wspolczynnik zmiany temperatury dla SW
            {
                do {

                    cout << endl << "Wybierz wspolczynnik zmiany temperatury a dla algorytmu SW:" << endl;
                    cout << "----------------------------------------------------------------------------------" << endl;
                    cout << "1) a = 0.995" << endl;
                    cout << "2) a = 0.99" << endl;
                    cout << "3) a = 0.9" << endl;
                    cout << "Wybor:";
                    cin >> choiceAlfa;

                    if(choiceAlfa < 1 || choiceAlfa > 3) {
                        cout << endl << "Nieprawidlowy numer wybranej opcji. Sprobuj ponownie." << endl;
                        choiceAlfa = 0;
                    }

                } while (choiceAlfa == 0);

                switch (choiceAlfa) {
                    case 1:
                        simulatedAnnealing.setAlfa(0.995);
                        break;
                    case 2:
                        simulatedAnnealing.setAlfa(0.99);
                        break;
                    case 3:
                        simulatedAnnealing.setAlfa(0.9);
                        break;
                    default:
                        cout << "Wystapil blad. Przypisano wartosc domyslna wspolczynnika alfa (0.99)." << endl;
                        simulatedAnnealing.setAlfa(0.99);
                        break;
                }

                cout << endl << "Wartosc wspolczynnika alfa zostala ustawiona." << endl;

                break;
            }

            case 7: // Algorytm Symulowanego Wyzarzania

                if (costMatrix != nullptr) {

                    simulatedAnnealing.setN(n);
                    simulatedAnnealing.setCostMatrix(costMatrix);

                    simulatedAnnealing.algorithm();
                    result = simulatedAnnealing.getResultVector();
                    cout << "Koszt znalezionego rozwiazania: " << simulatedAnnealing.calculatePathCost(result) << endl;
                    cout << "Znalezione rozwiazanie: ";
                    for(const int& vertice : result)
                        cout << vertice << " ";

                    //cout << "Wynik: " << tabuSearch.getResult() << endl;
                    //cout << "Najlepsza sciezka: ";
                    //tabuSearch.printResultVertices();
                    cout << endl;

                } else {
                    cout << endl << "Nie utworzono macierzy kosztow. Utworz macierz, aby wykonac algorytm." << endl;
                }

                break;

            case 8: // Zapis sciezki rozwiazania do pliku .txt

                if (!result.empty()) {

                    cout << endl << "Podaj nazwe pliku do zapisania sciezki rozwiazania: ";
                    cin >> filename;

                    std::ofstream outFile(filename);
                    if (!outFile) {
                        cerr << "Error: Nie mozna otworzyc pliku " << filename << endl;
                        return;
                    }

                    // Write the number of vertices (n) in the first line
                    outFile << result.size() << std::endl;

                    // Zapisz wierzchołki w kolejnych liniach
                    for (const int& vertex : result) {
                        outFile << vertex << "\n";
                    }

                    // Dodaj pierwszy wierzchołek na końcu, aby zamknąć cykl
                    outFile << result.front() << "\n";

                    outFile.close();
                    cout << endl << "Sciezka zapisana do pliku: " << filename << endl;

                } else {
                    cout << endl << "Wykonaj algorytm TS lub SW, aby zapisac wynik do pliku." << endl;
                }

                break;

            case 9: // wczytanie sciezki z pliku .txt
            {
                cout << endl << "Podaj nazwe pliku do odczytania sciezki rozwiazania: ";
                cin >> filename;

                std::ifstream inFile(filename);
                if (!inFile) {
                    cerr << "Error: Nie mozna otworzyc pliku " << filename << endl;
                    return;
                }

                int numVertices;
                // Read the number of vertices
                if (!(inFile >> numVertices)) {
                    std::cerr << "Error: Invalid file format (missing number of vertices)." << std::endl;
                    return;
                }

                // Read the path
                int *path = new int[n + 1];
                int vertex, index = 0;
                while (inFile >> vertex) {
                    path[index++] = vertex;
                }

                inFile.close();


                // Oblicz koszt drogi na podstawie macierzy kosztow
                int pathsCost = 0;
                for (int i = 0; i < numVertices; i++) {
                    pathsCost += costMatrix[path[i]][path[i + 1]];
                }
                pathsCost += costMatrix[path[n]][path[0]];

                cout << endl << "Odczytana sciezka:" << endl;
                for (int i = 0; i < numVertices + 1; i++) {
                    cout << path[i] << " ";
                }
                cout << endl << "Calkowity koszt scieki: " << pathsCost << endl;

                break;
            }
            case 10: // Zakoncz

                quit = true;
                break;

            default:
                cout << endl << "Niepoprawny numer wybranej opcji. Wybierz ponownie." << endl;
                break;
        }

    } while (!quit);
}

//-------------------------------------------------------------------------------------------------

int main() {
    showMenu();
    return 0;
}
