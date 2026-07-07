#include <iostream>
#include <vector>
#include "genetic/GeneticAlgorithm.h"

int main() {
    std::vector<std::vector<int>> costMatrix = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };
    GASettings settings;

    settings.populationSize = 10;
    settings.generations = 20;
    settings.crossoverProbability = 0.8;
    settings.mutationProbability = 0.1;
    settings.tournamentSize = 3;
    settings.eliteCount = 2;

    GeneticAlgorithm ga;

    return 0;
}