#pragma once

#include <vector>
#include <random>

//один вариант решения
struct Individual {
    std::vector<int> chromosome; //назначение работ кандидатам
    int cost = 0;
    double fitness = 0.0;
};

struct GASettings {
    int populationSize = 100;
    int generations = 200; //количество поколений
    double crossoverProbability = 0.8;
    double mutationProbability = 0.1;
    int tournamentSize = 3;
    int eliteCount = 2;
};

//инфа об поколении
struct GenerationInfo {
    int generationNumber = 0;
    Individual bestIndividual;
    int bestCost = 0;
};

struct GAResult {
    Individual bestIndividual;
    std::vector<GenerationInfo> history;
};

class GeneticAlgorithm{
public:
    GAResult run(const std::vector<std::vector<int>>& costMatrix, const GASettings& settings);

private:
    std::mt19937 rng{std::random_device{}()};

    int randomInt(int left, int right);

    double randomDouble(double left, double right);

    //вычисление стоимости решения
    int calculateCost(const std::vector<int>& chromosome, const std::vector<std::vector<int>>& costMatrix);

    //вычисление числа приспособленности
    double calculateFitness(int cost);

    void evaluateIndividual(Individual& individual, const std::vector<std::vector<int>>& costMatrix);

    //создание начальной популяции
    std::vector<Individual> createInitialPopulation(
        const std::vector<std::vector<int>>& costMatrix,
        int populationSize
    );

    std::vector<int> createRandomChromosome(int n);

    //создание решения + оценка его
    Individual createRandomIndividual(const std::vector<std::vector<int>>& costMatrix);


    Individual getBestIndividual(const std::vector<Individual>& population);

    //турнирный отбор
    Individual tournamentSelection(const std::vector<Individual>& population, int tournamentSize);


    //мутация(будет перестановка случайных работ)
    void mutateSwap(Individual& individual);

    //упорядоченное скрещивание
    Individual orderedCrossover(const Individual& parent1, const Individual& parent2);

    //создание нового поколения, исходя из старого
    std::vector<Individual> createNextGeneration(std::vector<Individual>& population, const std::vector<std::vector<int>>& costMatrix,
        const GASettings& settings);
};