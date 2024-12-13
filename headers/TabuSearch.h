
#ifndef PEA_ZAD2_TABUSEARCH_H
#define PEA_ZAD2_TABUSEARCH_H

#endif //PEA_ZAD2_TABUSEARCH_H


#include "Algorithm.h"
#include <iostream>
#include <vector>
#include <set>
#include <list>

using namespace std;

class TabuSearch : public Algorithm {

private:

    // Kryterium stopu
    int timeLimit;
    int iterationsWithoutImprovementLimit;

    // Definicja sasiedztwa; 1 - insert, 2 - swap, 3 - invert
    int neighborhoodDef;

    // Licznik iteracji bez poprawy rozwiazania
    int noImprovementIterations;

    // Lista tabu
    //std::set<std::pair<int, int>> tabuList;
    //std::list<std::pair<std::vector<int>, int>> tabuList;
    int** tabuList;

    // Kadencja dla rozwiazan na liscie tabu
    int tabuCadency;

    vector<int> randomPermutation(int size);
    void updateTabuList(/*const std::vector<int>& solution*/ int bestI, int bestJ);
    void diversify(std::vector<int>& currentSolution);
    bool isTabu(const std::vector<int>& solution);

    // Generowanie rozwiazan w sasiedztwa rozwiazania currentSolution, w zaleznosci od definicji
    std::vector<std::vector<int>> neighborhoodInsert(const std::vector<int>& solution);
    std::vector<std::vector<int>> neighborhoodSwap(const std::vector<int>& solution);
    std::vector<std::vector<int>> neighborhoodInvert(const std::vector<int>& solution);

    std::vector<int> findBestNeighborInsert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);
    std::vector<int> findBestNeighborSwap(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);
    std::vector<int> findBestNeighborInvert(const std::vector<int>& solution, int& bestCost, int& bestI, int& bestJ);


public:

    TabuSearch();
    void setTimeLimit(int t);
    void setNeighborhoodDef(int nDef);
    void algorithm();
    int calculatePathCost(vector <int> path);

};