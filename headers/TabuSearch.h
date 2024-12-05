
#ifndef PEA_ZAD2_TABUSEARCH_H
#define PEA_ZAD2_TABUSEARCH_H

#endif //PEA_ZAD2_TABUSEARCH_H


#include "Algorithm.h"
#include <iostream>
#include <vector>

using namespace std;

class TabuSearch : public Algorithm {

private:

    int time = 0;

    vector<int> randomPermutation(int size);
    int calculatePath(vector <int> path);

public:

    TabuSearch();
    void setTime(int t);
    void algorithm();

};