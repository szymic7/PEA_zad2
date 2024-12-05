#include "../headers/TabuSearch.h"

#include <ctime>
//#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <random>

//--------------------------------------------------------------------------------------------------

TabuSearch::TabuSearch() : Algorithm()
{
    time = 0;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::setTime(int t) {
    time = t;
}

//--------------------------------------------------------------------------------------------------

void TabuSearch::algorithm()
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
                    int currentCost = calculatePath(permutation);

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

                    if (timePassed >= time)
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
}

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

//--------------------------------------------------------------------------------------------

int TabuSearch::calculatePath(vector<int> path)
{
    int cost = 0;

    for (int i = 0; i < path.size() - 1; ++i)
    {
        cost += costMatrix[path[i]][path[i + 1]];
    }
    cost += costMatrix[path[n-1]][path[0]];

    return cost;
}

//---------------------------------------------------------------------------------------------
