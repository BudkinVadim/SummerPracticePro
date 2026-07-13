#include "RandomMatrixGenerator.h"

#include <random>
#include <stdexcept>

// Создание случайной квадратной матрицы
std::vector<std::vector<int>> RandomMatrixGenerator::generate(int size,int minCost,int maxCost){
    validateParameters(size, minCost, maxCost);

    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<int> distribution(minCost, maxCost);

    std::vector<std::vector<int>> matrix(size ,std::vector<int>(size));
    for (int r = 0; r < size; r++){
        for (int c = 0; c < size; c++){
            matrix[r][c] = distribution(generator);
        }
    }

    return matrix;
}

// Проверка параметров генерации.
void RandomMatrixGenerator::validateParameters(int size,int minCost,int maxCost){
    if (size <= 0){
        throw std::invalid_argument("Matrix size must be positive");
    }
    if (minCost <= 0){
        throw std::invalid_argument("Min cost must be positive");
    }

    if (maxCost < minCost){
        throw std::invalid_argument("Min > Max");
    }
}