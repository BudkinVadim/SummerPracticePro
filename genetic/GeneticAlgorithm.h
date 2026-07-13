#pragma once

#include <vector>
#include <random>

enum CrossoverType {
    OrderedCrossover,
    PositionalCrossing
};

//тип отбора
enum SelectionType {
    TournamentSelection,
    RouletteSelection
};

//тип мутации
enum MutationType {
    SwapMutation,
    InversionMutation
};

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
    int maxGenerationsWithoutImprovement = 0;
    int roulettePointerCount = 0;

    SelectionType selectionType = TournamentSelection;
    MutationType mutationType = SwapMutation;
    CrossoverType crossoverType = OrderedCrossover;
};

//инфа об одном поколении
struct GenerationInfo {
    //номер поколения
    int generationNumber = 0;

    //все решения текущего поколения
    std::vector<Individual> population;

    //количество особей в поколении
    int individualCount = 0;

    //лучшее решение в поколении
    Individual bestIndividual;

    //стоимость лучшей особи поколения
    int bestCost = 0;
};

struct GAResult {
    //лучшая особь среди всех поколений
    Individual bestIndividual;

    std::vector<GenerationInfo> history;

    //получение поколения по его индексу
    const GenerationInfo &getGeneration(int index) const;
};

class GeneticAlgorithm {
public:
    GAResult run(const std::vector<std::vector<int> > &costMatrix, const GASettings &settings,
                 float &loading_percentage, bool &stop);

    GAResult run(const std::vector<std::vector<int>> & vector, const GASettings & settings);

private:
    std::mt19937 rng{std::random_device{}()};

    int randomInt(int left, int right);

    double randomDouble(double left, double right);

    //вычисление стоимости решения
    int calculateCost(const std::vector<int> &chromosome, const std::vector<std::vector<int> > &costMatrix);

    //вычисление числа приспособленности
    double calculateFitness(int cost);

    void evaluateIndividual(Individual &individual, const std::vector<std::vector<int> > &costMatrix);

    //создание начальной популяции
    std::vector<Individual> createInitialPopulation(
        const std::vector<std::vector<int> > &costMatrix,
        int populationSize
    );

    std::vector<int> createRandomChromosome(int n);

    //создание решения + оценка его
    Individual createRandomIndividual(const std::vector<std::vector<int> > &costMatrix);


    Individual getBestIndividual(const std::vector<Individual> &population);

    //турнирный отбор
    Individual tournamentSelection(const std::vector<Individual> &population, int tournamentSize);

    //отбор стохастической рулеткой
    std::vector<Individual> stochasticRouletteSelection(const std::vector<Individual> &population, int pointerCount);

    //общий выбор родителя по настройкам
    Individual selectParent(const std::vector<Individual> &population, const GASettings &settings);

    //мутация(будет перестановка случайных работ)
    void mutateSwap(Individual &individual);

    //мутация(разворачивание подмассива внутри хромосомы)
    void mutateInversion(Individual &individual);

    //общий выбор мутациии по настройкам
    void mutateIndividual(Individual &individual, const GASettings &settings);

    //упорядоченное скрещивание
    Individual orderedCrossover(const Individual &parent1, const Individual &parent2);

    //позиционное скрещивание
    Individual positionCrossover(const Individual &parent1, const Individual &parent2);

    //общий выбор скрещивания по настройкам
    Individual crossoverIndividuals(const Individual &parent1, const Individual &parent2, const GASettings &settings);

    //создание нового поколения, исходя из старого
    std::vector<Individual> createNextGeneration(std::vector<Individual> &population,
                                                 const std::vector<std::vector<int> > &costMatrix,
                                                 const GASettings &settings);

    void validateSettings(const GASettings &settings);
};
