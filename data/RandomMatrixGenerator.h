#pragma once

#include <vector>

class RandomMatrixGenerator{
public:
    //cоздание рандомной квадратной матрицы в заданном диапозоне
    static std::vector<std::vector<int>> generate(int size,int minCost = 1, int maxCost = 1000);
private:
    //проверка матрицы на валидность
    static void validateParameters(int size,int minCost,int maxCost);
};