#include "GeneticAlgorithm.h"

#include <algorithm>
#include <numeric>
#include <limits>

//генерация целого
int GeneticAlgorithm::randomInt(int left, int right) {
    std::uniform_int_distribution<int> dist(left, right);
    return dist(rng);
}

//генерация дробного
double GeneticAlgorithm::randomDouble(double left, double right){
    std::uniform_real_distribution<double> dist(left, right);
    return dist(rng);
}

//стоимость решения
int GeneticAlgorithm::calculateCost(const std::vector<int>& chromosome, const std::vector<std::vector<int>>& costMatrix){
    int total = 0;

    for (int cand = 0; cand < chromosome.size(); cand++){
        total += costMatrix[cand][chromosome[cand]];
    }

    return total;
}

//число приспособленности
double GeneticAlgorithm::calculateFitness(int cost){
    return 1.0 / (1.0 + cost);
}

//оценка особи
void GeneticAlgorithm::evaluateIndividual(Individual& individual, const std::vector<std::vector<int>>& costMatrix) {
    individual.cost = calculateCost(individual.chromosome, costMatrix);
    individual.fitness = calculateFitness(individual.cost);
}

//создание рандомной хромосомы
std::vector<int> GeneticAlgorithm::createRandomChromosome(int n) {
    std::vector<int> chromosome(n);
    for (int i = 0; i < n; i++) {
        chromosome[i] = i;
    }

    std::shuffle(chromosome.begin(), chromosome.end(), rng);
    return chromosome;
}

//создание рандомной особи
Individual GeneticAlgorithm::createRandomIndividual(const std::vector<std::vector<int>>& costMatrix) {
    int n = costMatrix.size();
    Individual individual;

    individual.chromosome = createRandomChromosome(n);
    evaluateIndividual(individual, costMatrix);

    return individual;
}

//начальная популяция
std::vector<Individual> GeneticAlgorithm::createInitialPopulation(const std::vector<std::vector<int>>& costMatrix, int populationSize){
    std::vector<Individual> population(populationSize); 

    for (int i = 0; i < populationSize; i++) {
        population[i] = createRandomIndividual(costMatrix); 
    }

    return population;
}

//поиск лучшего индивидума в популяции
Individual GeneticAlgorithm::getBestIndividual(
    const std::vector<Individual>& population
) {
    Individual best = population[0];

    for (const Individual& individual : population) {
        if (individual.cost < best.cost) {
            best = individual;
        }
    }

    return best;
}

//турнир
Individual GeneticAlgorithm::tournamentSelection(const std::vector<Individual>& population, int tournamentSize){
    int bestIndex = randomInt(0, population.size() - 1);

    for (int i = 1; i < tournamentSize; i++) {
        int index = randomInt(0, population.size() - 1);

        if (population[index].cost < population[bestIndex].cost) {
            bestIndex = index;
        }
    }

    return population[bestIndex];
}

//мутация
void GeneticAlgorithm::mutateSwap(Individual& individual) {
    int n = individual.chromosome.size();

    int first = randomInt(0, n - 1);
    int second = randomInt(0, n - 1);

    std::swap(individual.chromosome[first], individual.chromosome[second]);
}
