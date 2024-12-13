#include "../headers/TabuSearch.h"
#include "../headers/Greedy.h"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

//--------------------------------------------------------------------------------------------------

TabuSearch::TabuSearch() : Algorithm()
{
    timeLimit = 0;
    iterationsWithoutImprovementLimit = 300;

    neighborhoodDef = 1;
    noImprovementIterations = 0;
    tabuCadency = 25;

    tabuList = nullptr;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setTimeLimit(int t) {
    timeLimit = t;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setNeighborhoodDef(int nDef) {
    neighborhoodDef = nDef;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::algorithm() {

    // Wyczyszczenie rozwiazania
    resultVector.clear();
    double timeOfResult = 0.0;
    noImprovementIterations = 0;

    // Wyczyszczenie listy tabu
    if (tabuList != nullptr) {
        for (int i = 0; i < n; ++i) {
            delete [] tabuList[i];
        }
        delete [] tabuList;
    }

    // Inicjalizacja nowej listy tabu
    tabuList = new int*[n];
    for (int i = 0; i < n; ++i) {
        tabuList[i] = new int[n];
        for(int j = 0; j < n; j++) {
            tabuList[i][j] = 0; // Poczatkowo tabu list wypelniona wartosciami 0
        }
    }

    // Rozwiazanie poczatkowe - wygenerowane metoda losowa
    std::vector<int> currentSolution;
    /*currentSolution.resize(n);
    std::iota(currentSolution.begin(), currentSolution.end(), 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);*/
    Greedy greedy;
    greedy.setN(n);
    greedy.setCostMatrix(costMatrix);
    greedy.greedyAlgorithm();

    int* resultVertices = greedy.getResultVertices();
    currentSolution.resize(n); // Ustaw rozmiar wektora
    for (int i = 0; i < n; ++i) {
        currentSolution[i] = resultVertices[i];
    }
    delete [] resultVertices;


    cout << "Rozwiazanie poczatkowe: ";
    for (int i : currentSolution) {
        cout << i << " ";
    }
    cout << endl;

    std::vector<int> bestSolution = currentSolution;
    int bestCost = calculatePathCost(bestSolution);

    // Wierzcholki i, j do dodania do listy tabu
    int bestI, bestJ;

    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {
        int currentCost = std::numeric_limits<int>::max();
        std::vector<int> bestNeighbor;

        //std::vector<std::vector<int>> neighbors;

        switch (neighborhoodDef) {
            case 1: // insert
                //neighbors = neighborhoodInsert(currentSolution);
                bestNeighbor = findBestNeighborInsert(currentSolution, currentCost, bestI, bestJ);
                break;
            case 2: // swap
                //neighbors = neighborhoodSwap(currentSolution);
                bestNeighbor = findBestNeighborSwap(currentSolution, currentCost, bestI, bestJ);
                break;
            case 3: // invert
                //neighbors = neighborhoodInvert(currentSolution);
                bestNeighbor = findBestNeighborInvert(currentSolution, currentCost, bestI, bestJ);
                break;
            default:
                //neighbors = neighborhoodInsert(currentSolution);
                bestNeighbor = findBestNeighborInsert(currentSolution, currentCost, bestI, bestJ);
                cout << endl << "Ustawiono domyslna definicje sasiedztwa - Insert." << endl;
                break;
        }

        currentSolution = bestNeighbor;

        /*for(const auto& neighbor : neighbors) {
            if (!isTabu(neighbor)) {
            //if (tabuList.count({neighbor.front(), neighbor.back()}) == 0) {
                int neighborCost = calculatePathCost(neighbor);
                if (neighborCost < currentCost) {
                    currentCost = neighborCost;
                    bestNeighbor = neighbor;
                }
            }
        }*/

        if (currentCost < bestCost) { // Nowe rozwiazanie jest dotychczas najlepszym
            bestCost = currentCost;
            bestSolution = bestNeighbor;
            noImprovementIterations = 0;
            timeOfResult = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
        } else {
            noImprovementIterations++;
        }

        // Aktualizacja listy tabu
        updateTabuList(bestI, bestJ);

        // Ewentualna dywersyfikacja
        if (noImprovementIterations >= iterationsWithoutImprovementLimit) {
            diversify(currentSolution); // Wymuś dywersyfikację
        }



        //updateTabuList(bestNeighbor);

    }

    resultVector = bestSolution;

    cout << "Czas znalezienia najlepszego rozwiazania: " << timeOfResult << " sekund" << endl;
}

//---------------------------------------------------------------------------------------------

std::vector<std::vector<int>> TabuSearch::neighborhoodInsert(const std::vector<int>& solution) {
    std::vector<std::vector<int>> neighbors;
    for (int i = 0; i < solution.size(); i++) {
        for (int j = 0; j < solution.size(); j++) {
            if (i != j) {
                std::vector<int> neighbor = solution;
                int temp = neighbor[i];
                neighbor.erase(neighbor.begin() + i);
                neighbor.insert(neighbor.begin() + j, temp);
                neighbors.push_back(neighbor);
            }
        }
    }
    return neighbors;
}


//---------------------------------------------------------------------------------------------

std::vector<std::vector<int>> TabuSearch::neighborhoodSwap(const std::vector<int>& solution) {
    std::vector<std::vector<int>> neighbors;
    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {
            std::vector<int> neighbor = solution;
            std::swap(neighbor[i], neighbor[j]);
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

//---------------------------------------------------------------------------------------------

std::vector<std::vector<int>> TabuSearch::neighborhoodInvert(const std::vector<int>& solution) {
    std::vector<std::vector<int>> neighbors;
    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {
            std::vector<int> neighbor = solution;
            std::reverse(neighbor.begin() + i, neighbor.begin() + j + 1);
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborInsert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor = solution;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = 0; j < solution.size(); j++) {
            if (i != j) {
                neighbor = solution;

                int temp = neighbor[i];
                neighbor.erase(neighbor.begin() + i);

                // Skoryguj pozycję `j`, jeśli jest większa niż `i`
                int adjustedJ = j;
                if (j > i) {
                    adjustedJ--; // Po usunięciu elementu, indeks `j` przesuwa się o 1 w lewo
                }

                neighbor.insert(neighbor.begin() + adjustedJ, temp);

                int cost = calculatePathCost(neighbor);
                if (/*!isTabu(neighbor)*/ tabuList[i][j] == 0 || cost < bestCost) {
                    if (cost < bestCost) {
                        bestCost = cost;
                        bestNeighbor = neighbor;
                        bestI = i;
                        bestJ = j;
                    }
                }
            }
        }
    }

    //cout << endl << "Zakoczono przeszukiwanie sasiedztwa." << endl;

    return bestNeighbor;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborSwap(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor = solution;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {
            std::swap(neighbor[i], neighbor[j]);

            int cost = calculatePathCost(neighbor);
            if (/*!isTabu(neighbor)*/ tabuList[i][j] == 0 || cost < bestCost) {
                if (cost < bestCost) {
                    bestCost = cost;
                    bestNeighbor = neighbor;
                    bestI = i;
                    bestJ = j;
                }
            }

            // Przywroc stan
            std::swap(neighbor[i], neighbor[j]);
        }
    }

    //cout << endl << "Zakoczono przeszukiwanie sasiedztwa." << endl;

    return bestNeighbor;
}

//---------------------------------------------------------------------------------------------

std::vector<int> TabuSearch::findBestNeighborInvert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ) {
    std::vector<int> bestNeighbor = solution;
    bestCost = std::numeric_limits<int>::max();
    std::vector<int> neighbor = solution;

    for (int i = 0; i < solution.size(); i++) {
        for (int j = i + 1; j < solution.size(); j++) {
            std::reverse(neighbor.begin() + i, neighbor.begin() + j + 1);

            int cost = calculatePathCost(neighbor);
            if (/*!isTabu(neighbor)*/ tabuList[i][j] == 0 || cost < bestCost) {
                if (cost < bestCost) {
                    bestCost = cost;
                    bestNeighbor = neighbor;
                    bestI = i;
                    bestJ = j;
                }
            }

            // Przywróć stan
            std::reverse(neighbor.begin() + i, neighbor.begin() + j + 1);
        }
    }

    //cout << endl << "Zakoczono przeszukiwanie sasiedztwa." << endl;

    return bestNeighbor;
}

/*std::vector<int> TabuSearch::findBestNeighborInvert(const std::vector<int>& solution, int& bestCost) {
    int size = solution.size();

    // Konwersja wektora na dynamiczną tablicę
    int* currentNeighbor = new int[size];
    int* bestNeighbor = new int[size];
    std::copy(solution.begin(), solution.end(), currentNeighbor);
    std::copy(solution.begin(), solution.end(), bestNeighbor);

    bestCost = std::numeric_limits<int>::max();

    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            // Odwrócenie fragmentu w dynamicznej tablicy
            int start = i, end = j;
            while (start < end) {
                std::swap(currentNeighbor[start], currentNeighbor[end]);
                start++;
                end--;
            }

            // Obliczenie kosztu dla bieżącego sąsiada
            int cost = calculatePathCost(currentNeighbor);
            if (!isTabu(currentNeighbor) || cost <= bestCost) {
                if (cost < bestCost) {
                    bestCost = cost;
                    std::copy(currentNeighbor, currentNeighbor + size, bestNeighbor);
                }
            }

            // Przywrócenie stanu (odwrócenie ponowne)
            start = i;
            end = j;
            while (start < end) {
                std::swap(currentNeighbor[start], currentNeighbor[end]);
                start++;
                end--;
            }
        }
    }

    // Konwersja najlepszej tablicy z powrotem na wektor
    std::vector<int> bestNeighborVector(bestNeighbor, bestNeighbor + size);

    // Zwolnienie pamięci dynamicznej
    delete[] currentNeighbor;
    delete[] bestNeighbor;

    return bestNeighborVector;
}*/


//---------------------------------------------------------------------------------------------

void TabuSearch::updateTabuList(/*const std::vector<int>& solution*/ int bestI, int bestJ) {

    /*
    for (auto it = tabuList.begin(); it != tabuList.end(); ) {
        it -> second--; // Decrease remaining iterations
        if (it -> second <= 0) {
            it = tabuList.erase(it); // Remove expired tabu entries
        } else {
            it++;
        }
    }

    // Add the new solution to the tabu list with the tabuCadency
    tabuList.emplace_back(solution, tabuCadency);
     */

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (tabuList[i][j] > 0) tabuList[i][j]--;
        }
    }

    // Dodanie ostatniego ruchu do listy tabu
    tabuList[bestI][bestJ] = tabuCadency;

    // Dla operacji Swap i Invert ruch (i, j) jest rownoznaczny ruchowi (j, i)
    if(neighborhoodDef == 2 || neighborhoodDef == 3) {
        tabuList[bestJ][bestI] = tabuCadency;
    }
}

//---------------------------------------------------------------------------------------------

bool TabuSearch::isTabu(const std::vector<int>& solution) {

    /*for (const auto& tabuEntry : tabuList) {
        if (tabuEntry.first == solution) {
            return true; // The solution is in the tabu list
        }
    }
    return false; // The solution is not tabu*/


    /*for (const auto& tabuEntry : tabuList) {
        const auto& tabuSolution = tabuEntry.first;

        if (solution.size() != tabuSolution.size()) {
            continue; // Skip if the sizes are not the same
        }

        // Check cyclic permutation
        auto it = std::find(tabuSolution.begin(), tabuSolution.end(), solution[0]);
        if (it != tabuSolution.end()) {
            // Found starting point; check cyclic equivalence
            size_t startIdx = std::distance(tabuSolution.begin(), it);
            bool isCyclicMatch = true;

            for (size_t i = 0; i < solution.size(); ++i) {
                if (solution[i] != tabuSolution[(startIdx + i) % solution.size()]) {
                    isCyclicMatch = false;
                    break;
                }
            }

            if (isCyclicMatch) {
                return true; // Found a match in the tabu list
            }
        }
    }

    return false; // No cyclic match found*/

}

//---------------------------------------------------------------------------------------------

void TabuSearch::diversify(std::vector<int>& currentSolution) {

    /*std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);
    noImprovementIterations = 0;*/

    // Wprowadzenie subtelnych zmian w trasie
    std::random_device rd;
    std::mt19937 gen(rd());
    int numPerturbations = n / 10; // Zmieniamy 10% trasy
    for (int k = 0; k < numPerturbations; ++k) {

        int i = gen() % (n - 1) + 1;
        int j;
        do {
            j = gen() % (n - 1) + 1;
        } while (i == j);
        std::swap(currentSolution[i], currentSolution[j]); // Zamiana dwóch losowych miast

    }
    noImprovementIterations = 0;
}

//---------------------------------------------------------------------------------------------

int TabuSearch::calculatePathCost(vector<int> path) {
    int cost = 0;

    for (int i = 0; i < path.size() - 1; i++) {
        cost += costMatrix[path[i]][path[i + 1]];
    }
    cost += costMatrix[path[n-1]][path[0]];

    return cost;
}

//---------------------------------------------------------------------------------------------

