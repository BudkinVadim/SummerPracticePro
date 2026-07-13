#ifndef SP_WORK_GUI_H
#define SP_WORK_GUI_H
#include "../lib/raylib.h"
#include "../lib/tinyfiledialogs.h"

#include "../genetic/GeneticAlgorithm.h"
#include "../data/RandomMatrixGenerator.h"
#include "../data/MatrixReader.h"

#include "../ui_utils/Button.h"
#include "../ui_utils/NumberBox.h"
#include "../ui_utils/Table.h"

#include <future>

enum Screen {
    EXIT,
    MENU,
    LOADING,
    MTX_CREATION,
    VISUALIZATION,
};

// Вариации экрана
class GUI {
public:
    // Текущая вариация экрана
    Screen screen;

    // Данные алгоритма
    std::vector<std::vector<int> > costMatrix;
    GASettings settings;
    GAResult result;

    std::future<void> gaFuture;
    bool gaRunning = false;
    bool stopAlgorithm = false;
    float loading_percentage = 0;

    //std::string getGAResult();

    // Отображение меню
    void menu_screen(std::vector<NumberBox> &nbs, std::string &errorMessage);

    // Отображение экрана визуализации
    void visualization_screen(std::vector<NumberBox> &nbs);

    // Отображение экрана создания матрицы
    void mtx_creation_screen(std::vector<NumberBox> &nbs, std::string &errorMessage);

    // Отображение экрана загрузки
    void loading_screen(std::vector<NumberBox> &nbs, std::string &errorMessage);

    // Функция добавления точки из вектора точек
    void addPoint(std::vector<Vector2> &points, const Rectangle &graphArea, Scaling &scale, int &currStep);

    // Функция удаления точки из вектора точек
    void removePoint(std::vector<Vector2> &points, const Rectangle &graphArea, Scaling &scale, int &currStep);

    void updateTable(Table &table, int &currStep, int &currIndividual);
};

#endif //SP_WORK_GUI_H
