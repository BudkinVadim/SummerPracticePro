#pragma once

#include <vector>
#include <string>

class MatrixReader {
public:
    //обработка матрицы из файла
    static std::vector<std::vector<int>> read(const std::string& filename);

private:
    //построчное чтение
    static std::vector<int> parseLine(const std::string& line);
    static void validateMatrix(const std::vector<std::vector<int>>& matrix);
};