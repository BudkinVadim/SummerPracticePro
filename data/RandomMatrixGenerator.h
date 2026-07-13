#pragma once

#include <vector>

class RandomMatrixGenerator{
public:
    //создание случайной квадратной матрицы в заданном диапазоне
    static std::vector<std::vector<int>> generate(int size,int minCost = 1, int maxCost = 1000);
private:
    //проверка матрицы на валидность
    static void validateParameters(int size,int minCost,int maxCost);
};