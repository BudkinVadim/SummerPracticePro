#ifndef SP_WORK_TABLE_H
#define SP_WORK_TABLE_H
#include "../lib/raylib.h"
#include <string>
#include  <vector>
#include <stdexcept>

class Table {
private:
    std::vector<std::vector<std::string> > data;
    int currentPage;
    int rows;
    int columns;
    int visibleRows;

    int cellWidth;
    int cellHeight;

public:
    Table(int columns, int rows, int visibleRows, int cellWidth, int cellHeight);

    int getColumns() { return columns; };
    int getRows() { return rows; };

    void addColumn(std::vector<std::string> columnData);

    void addRow(std::vector<std::string> rowData);

    void setColumn(int index, std::vector<std::string> columnData);

    void setRow(int index, std::vector<std::string> rowData);

    void nextPage();

    void prevPage();

    void draw(int x, int y);
};


#endif //SP_WORK_TABLE_H
