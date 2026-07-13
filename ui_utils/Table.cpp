#include "Table.h"


Table::Table(int columns, int rows, int visibleRows, int cellWidth, int cellHeight)
    : columns(columns), rows(rows), currentPage(0), visibleRows(visibleRows),
      cellHeight(cellHeight), cellWidth(cellWidth) {
    currentPage = 0;
    data.clear();
    for (size_t i = 0; i < rows; ++i) {
        data[i] = std::vector<std::string>(columns);
    }
}


void Table::addColumn(std::vector<std::string> columnData) {
    if (columnData.size() != rows) throw std::invalid_argument("Incorrect number of columns");
    columns++;
    for (size_t i = 0; i < rows; ++i) {
        data[i].push_back(columnData[i]);
    }
}

void Table::addRow(std::vector<std::string> rowData) {
    if (rowData.size() != columns) throw std::invalid_argument("Incorrect number of rows");
    rows++;
    data.push_back(rowData);
}

void Table::setColumn(int index, std::vector<std::string> columnData) {
    if (columnData.size() != rows) throw std::invalid_argument("Incorrect number of columns");
    columns++;
    for (size_t i = 0; i < columns; ++i) {
        data[i][index] = columnData[i];
    }
}

void Table::setRow(int index, std::vector<std::string> rowData) {
    if (rowData.size() != columns) throw std::invalid_argument("Incorrect number of rows");
    data[index] = rowData;
}

void Table::nextPage() {
    if ((currentPage + 1) * visibleRows > rows) return;
    currentPage++;
}

void Table::prevPage() {
    if (currentPage < 1) return;
    currentPage--;
}

void Table::draw(int x, int y) {
    for (int i = 0; i < visibleRows; ++i) {
        for (int j = 0; j < columns; ++j) {
            int row = currentPage * visibleRows + i;

            if (row >= rows)
                break;

            int Xcell = x + j * cellWidth;
            int Ycell = y + i * cellHeight;

            DrawRectangleLines(Xcell, Ycell, cellWidth, cellHeight, BLACK);

            // Текст внутри ячейки
            DrawText(data[row][j].c_str(), Xcell + 5, Ycell + 8, 18, BLACK);
        }
    }
}
