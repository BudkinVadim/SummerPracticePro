#include <iostream>
#include <vector>
#include <exception>

#include "genetic/GeneticAlgorithm.h"
#include "data/MatrixReader.h"

int main() {
    try {
        std::vector<std::vector<int>> costMatrix = MatrixReader::read("matrix.csv");

        GASettings settings;
        settings.populationSize = 5;
        settings.generations = 50;
        settings.crossoverProbability = 0.8;
        settings.mutationProbability = 0.1;
        settings.tournamentSize = 3;
        settings.eliteCount = 2;
        settings.maxGenerationsWithoutImprovement = 50;
        settings.roulettePointerCount = 0;
        settings.selectionType = RouletteSelection;
        settings.mutationType = SwapMutation;
        settings.crossoverType = OrderedCrossover;

        GeneticAlgorithm ga;
        GAResult result = ga.run(costMatrix, settings);

        std::cout << "Best chromosome: ";

        for (int gene : result.bestIndividual.chromosome) {
            std::cout << gene << " ";
        }

        std::cout << std::endl;
        std::cout << "Best cost: " << result.bestIndividual.cost << std::endl;

        std::cout << std::endl;
        std::cout << "Best assignment:" << std::endl;

        for (int candidate = 0; candidate < result.bestIndividual.chromosome.size(); candidate++) {
            int job = result.bestIndividual.chromosome[candidate];
            std::cout << "Candidate " << candidate << " -> Job " << job << std::endl;
        }

        std::cout << std::endl;
        std::cout << "History:" << std::endl;

        for (const GenerationInfo& info : result.history) {
            std::cout << "Generation " << info.generationNumber
                      << " | best cost = " << info.bestCost
                      << std::endl;
        }

    }catch(const std::exception& error) {
        std::cout << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}