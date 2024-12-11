#include "../headers/TabuSearch.h"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

//--------------------------------------------------------------------------------------------------

TabuSearch::TabuSearch() : Algorithm()
{
    timeLimit = 0;
    iterationsWithoutImprovementLimit = 110;

    neighborhoodDef = 1;
    noImprovementIterations = 0;
    tabuCadency = 10;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setTimeLimit(int t) {
    timeLimit = t;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setNeighborhoodDef(int nDef) {
    neighborhoodDef = nDef;
}

//--------------------------------------------------------------------------------------------------

/*void TabuSearch::run()
{
    vector<vector<int>> tabuMatrix;
    vector<int>best;
    vector<int>permutation = randomPermutation(n);
    vector<int>next(permutation);
    int result = 1 << 30;
    int firstToSwap;
    int secondToSwap;
    int nextCost;
    std::clock_t start;
    double timePassed;
    double foundTime;

    tabuMatrix.resize(n);

    for (int j = 0; j < n; ++j)
    {
        tabuMatrix[j].resize(n, 0);
    }

    start = std::clock();

    while(true)
    {
        for (int step = 0; step <  15 * n; ++step)
        {
            firstToSwap = 0;
            secondToSwap = 0;
            nextCost = 1 << 30;

            for (int first = 0; first < n; ++first)
            {
                for (int second = first + 1; second < n; ++second)
                {
                    std::swap(permutation[first], permutation[second]);
                    int currentCost = calculatePathCost(permutation);

                    if (currentCost < result)
                    {
                        result = currentCost;
                        best = permutation;
                        foundTime = (std::clock() - start) / (double)CLOCKS_PER_SEC;
                    }

                    if (currentCost < nextCost)
                    {
                        if (tabuMatrix[second][first]<step)
                        {
                            nextCost = currentCost;
                            next = permutation;

                            firstToSwap = second;
                            secondToSwap = first;
                        }
                    }

                    std::swap(permutation[first], permutation[second]);

                    timePassed = (std::clock() - start) / (double)CLOCKS_PER_SEC;

                    if (timePassed >= timeLimit)
                    {
                        cout << "Droga: ";
                        for (int d = 0; d < n; d++)
                        {
                            cout << best[d] << " ";
                        }

                        cout << "\nKoszt: " << result << endl;
                        cout << "Znaleziono po: " << foundTime << " s " << endl;
                        return;
                    }

                }
            }
            permutation = next;
            tabuMatrix[firstToSwap][secondToSwap] += n;
        }

        permutation.clear();

        permutation = randomPermutation(n);

        for (std::vector<vector<int> >::iterator it = tabuMatrix.begin(); it != tabuMatrix.end(); ++it)
        {
            it->clear();
        }
        tabuMatrix.clear();

        tabuMatrix.resize(n);

        for (std::vector<vector<int> >::iterator it = tabuMatrix.begin(); it != tabuMatrix.end(); ++it)
        {
            it->resize(n, 0);
        }
    }
}*/

//------------------------------------------------------------------------------------------------

vector<int> TabuSearch::randomPermutation(int _size)
{
    std::vector<int> temp;
    temp.reserve(_size);

    for (int i = 0; i < _size; i++)
    {
        temp.push_back(i);
    }

    shuffle(temp.begin(), temp.end(), std::mt19937(std::random_device()()));

    return temp;
}

//---------------------------------------------------------------------------------------------

void TabuSearch::algorithm() {

    // Wyczyszczenie rozwiazania
    resultVector.clear();

    // Rozwiazanie poczatkowe - wygenerowane metoda losowa
    std::vector<int> currentSolution;
    currentSolution.resize(n);
    std::iota(currentSolution.begin(), currentSolution.end(), 0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);

    std::vector<int> bestSolution = currentSolution;
    int bestCost = calculatePathCost(bestSolution);

    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeLimit) {
        int currentCost = std::numeric_limits<int>::max();
        std::vector<int> bestNeighbor;

        std::vector<std::vector<int>> neighbors;

        switch (neighborhoodDef) {
            case 1: // insert
                neighbors = neighborhoodInsert(currentSolution);
                break;
            case 2: // swap
                neighbors = neighborhoodSwap(currentSolution);
                break;
            case 3: // invert
                neighbors = neighborhoodInvert(currentSolution);
                break;
            default:
                neighbors = neighborhoodInsert(currentSolution);
                cout << endl << "Ustawiono domyslna definicje sasiedztwa - Insert." << endl;
                break;
        }

        for(const auto& neighbor : neighbors) {
            if (!isTabu(neighbor)) {
            //if (tabuList.count({neighbor.front(), neighbor.back()}) == 0) {
                int neighborCost = calculatePathCost(neighbor);
                if (neighborCost < currentCost) {
                    currentCost = neighborCost;
                    bestNeighbor = neighbor;
                }
            }
        }

        if (currentCost < bestCost) { // Nowe rozwiazanie jest dotychczas najlepszym
            bestCost = currentCost;
            bestSolution = bestNeighbor;
            noImprovementIterations = 0;
        } else {
            noImprovementIterations++;
        }

        if (noImprovementIterations >= iterationsWithoutImprovementLimit) {
            diversify(currentSolution); // Wymuś dywersyfikację
        }

        currentSolution = bestNeighbor;
        //updateTabuList({bestNeighbor.front(), bestNeighbor.back()});
        updateTabuList(bestNeighbor);
    }

    resultVector = bestSolution;
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

void TabuSearch::updateTabuList(const std::vector<int>& solution) {

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
}

//---------------------------------------------------------------------------------------------

bool TabuSearch::isTabu(const std::vector<int>& solution) {

    /*for (const auto& tabuEntry : tabuList) {
        if (tabuEntry.first == solution) {
            return true; // The solution is in the tabu list
        }
    }
    return false; // The solution is not tabu*/

    for (const auto& tabuEntry : tabuList) {
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

    return false; // No cyclic match found

}

//---------------------------------------------------------------------------------------------

void TabuSearch::diversify(std::vector<int> currentSolution) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(currentSolution.begin(), currentSolution.end(), gen);
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

