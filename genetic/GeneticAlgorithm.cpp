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

//стохастическая универсальная рулетка
std::vector<Individual> GeneticAlgorithm::stochasticRouletteSelection(const std::vector<Individual>& population,int pointerCount){
    std::vector<Individual> selectedParents;
    if (pointerCount <= 0) return selectedParents;

    double sumFitness = 0.0;

    for (const Individual& individual : population) {
        sumFitness += individual.fitness;
    }

    double step = sumFitness / pointerCount;

    double pointer = randomDouble(0.0, step);

    int currentIndex = 0;
    double currentSum = population[currentIndex].fitness;

    for (int i = 0; i < pointerCount; i++) {
        while (currentSum < pointer && currentIndex < population.size() - 1) {
            currentIndex++;
            currentSum += population[currentIndex].fitness;
        }

        selectedParents.push_back(population[currentIndex]);
        pointer += step;
    }

    return selectedParents;
}

Individual GeneticAlgorithm::selectParent(const std::vector<Individual>& population,const GASettings& settings) {
    return tournamentSelection(population, settings.tournamentSize);
}

//мутация перестановкой генов
void GeneticAlgorithm::mutateSwap(Individual& individual) {
    int n = individual.chromosome.size();

    int first = randomInt(0, n - 1);
    int second = randomInt(0, n - 1);

    std::swap(individual.chromosome[first], individual.chromosome[second]);
}

//мутация инверсией подмассива
void GeneticAlgorithm::mutateInversion(Individual& individual) {
    int n = individual.chromosome.size();
    int left = randomInt(0, n - 1);
    int right = randomInt(0, n - 1);

    if (left > right) std::swap(left, right);

    while (left < right) {
        std::swap(individual.chromosome[left], individual.chromosome[right]);
        left++;
        right--;
    }
}

//выбор мутации
void GeneticAlgorithm::mutateIndividual(Individual& individual, const GASettings& settings) {
    if (settings.mutationType == InversionMutation){
        mutateInversion(individual);
    }else{
        mutateSwap(individual);
    }
}

//упорядоченное скрещивание
Individual GeneticAlgorithm::orderedCrossover(const Individual& parent1,const Individual& parent2){
    int n = parent1.chromosome.size();

    Individual child;
    child.chromosome.assign(n, -1);

    int left = randomInt(0, n - 1);
    int right = randomInt(0, n - 1);

    if (left > right) {
        std::swap(left, right);
    }

    std::vector<bool> used(n, false);//для проверки занятости работы

    for (int i = left; i <= right; i++) {
        int gene = parent1.chromosome[i];
        child.chromosome[i] = gene;
        used[gene] = true;
    }

    int childPos = (right + 1) % n;

    for (int k = 0; k < n; k++) {
        int parent2Pos = (right + 1 + k) % n;
        int gene = parent2.chromosome[parent2Pos];

        if (used[gene]) {
            continue;
        }

        while (child.chromosome[childPos] != -1) {
            childPos = (childPos + 1) % n;
        }

        child.chromosome[childPos] = gene;
        used[gene] = true;

        childPos = (childPos + 1) % n;
    }

    return child;
}

//позиционное скрещивание
Individual GeneticAlgorithm::positionCrossover(const Individual& parent1, const Individual& parent2) {
    int n = parent1.chromosome.size();

    Individual child;
    child.chromosome.assign(n, -1);

    std::vector<bool> used(n, false);

    //выбираем случайные гены из 1 родителя
    for (int i = 0; i < n; i++) {
        if (randomInt(0, 1) == 1) {
            int gene = parent1.chromosome[i];

            child.chromosome[i] = gene;
            used[gene] = true;
        }
    }

    //заполняем пропуски генами 2 родителя
    int childPos = 0;

    for (int i = 0; i < n; i++) {
        int gene = parent2.chromosome[i];

        if (used[gene]) {
            continue;
        }

        while (childPos < n && child.chromosome[childPos] != -1) {
            childPos++;
        }

        if (childPos < n) {
            child.chromosome[childPos] = gene;
            used[gene] = true;
        }
    }

    return child;
}

//выбор скрещивания
Individual GeneticAlgorithm::crossoverIndividuals(const Individual& parent1, const Individual& parent2,
    const GASettings& settings) {
    if (settings.crossoverType == PositionalCrossing) {
        return positionCrossover(parent1, parent2);
    }
    return orderedCrossover(parent1, parent2);
}

//создание нового поколения
std::vector<Individual> GeneticAlgorithm::createNextGeneration(std::vector<Individual>& population, const std::vector<std::vector<int>>& costMatrix,
    const GASettings& settings) {
    std::vector<Individual> newPopulation;

    std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b){
            return a.cost < b.cost;
        }
    );

    //элитные особи переходят в след этап буз из
    for (int i = 0; i < settings.eliteCount && i < population.size(); i++) {
        newPopulation.push_back(population[i]);
    }

    std::vector<Individual> rouletteParents;
    int rouletteParentIndex = 0;

    if (settings.selectionType == RouletteSelection) {
        int childrenCount = settings.populationSize - newPopulation.size();

        int neededParentsCount = childrenCount * 2;

        int pointerCount = settings.roulettePointerCount;

        if (pointerCount <= 0) {
            pointerCount = neededParentsCount;
        }

        rouletteParents = stochasticRouletteSelection(population, pointerCount);
    }

    while (newPopulation.size() < settings.populationSize) {
        Individual parent1;
        Individual parent2;

        if (settings.selectionType == RouletteSelection) {
            parent1 = rouletteParents[rouletteParentIndex % rouletteParents.size()];
            rouletteParentIndex++;

            parent2 = rouletteParents[rouletteParentIndex % rouletteParents.size()];
            rouletteParentIndex++;
        } else {
            parent1 = selectParent(population, settings);
            parent2 = selectParent(population, settings);
        }


        Individual child;

        if (randomDouble(0.0, 1.0) < settings.crossoverProbability) {
            child = crossoverIndividuals(parent1, parent2, settings);
        } else {
            if (randomInt(0, 1) == 0) {
                child = parent1;
            } else {
                child = parent2;
            }
        }

        if (randomDouble(0.0, 1.0) < settings.mutationProbability) {
            mutateIndividual(child, settings);
        }

        evaluateIndividual(child, costMatrix);

        newPopulation.push_back(child);
    }

    return newPopulation;
}

//главная функция
GAResult GeneticAlgorithm::run(const std::vector<std::vector<int>>& costMatrix,const GASettings& settings) {
    GAResult result;

    std::vector<Individual> population = createInitialPopulation(costMatrix,settings.populationSize);

    Individual globalBest = getBestIndividual(population);

    int fail = 0;

    for (int generation = 0; generation <= settings.generations; generation++) {
        Individual currentBest = getBestIndividual(population);

        if (currentBest.cost < globalBest.cost) {
            globalBest = currentBest;
            fail = 0;
        }else {
            fail++;
        }

        GenerationInfo info;
        info.generationNumber = generation;
        info.bestIndividual = currentBest;
        info.bestCost = currentBest.cost;

        result.history.push_back(info);

        bool exitNoImprovement = settings.maxGenerationsWithoutImprovement > 0 && fail >= settings.maxGenerationsWithoutImprovement;

        if (exitNoImprovement) {
            break;
        }

        if (generation < settings.generations){
            population = createNextGeneration(
                population,
                costMatrix,
                settings
            );
        }
    }

    result.bestIndividual = globalBest;

    return result;
}
