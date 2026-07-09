#include "MatrixReader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>


std::vector<std::vector<int>> MatrixReader::read(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);

    std::vector<std::vector<int>> matrix;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<int> row = parseLine(line);
        matrix.push_back(row);
    }

    //проверяем, что матрица не пустая и квадратная
    validateMatrix(matrix);

    return matrix;
}


std::vector<int> MatrixReader::parseLine(const std::string& line){
    std::vector<int> row;
    //создание потока данных подобного ifstream
    std::stringstream ss(line);

    std::string value;

    if (!line.empty() && line.back() == ',') {
        throw std::runtime_error("Empty value in CSV line: " + line);
    }

    //пазбиение строки
    while (std::getline(ss, value, ',')) {

        if (value.empty()){
            throw std::runtime_error("Empty value in CSV line: " + line);
        }

        size_t position = 0;
        int number = std::stoi(value, &position);

        if (position != value.size()) {
            throw std::runtime_error("Invalid integer value: " + value);
        }

        if (number <= 0) {
            throw std::runtime_error("Cost values must be positive");
        }

        row.push_back(number);
    }

    return row;
}

void MatrixReader::validateMatrix(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty()) {
        throw std::runtime_error("Matrix is empty");
    }

    int n = matrix.size();

    for (int i = 0; i < n; i++) {
        if (matrix[i].size() != n) {
            throw std::runtime_error("Matrix must be square");
        }
    }
}