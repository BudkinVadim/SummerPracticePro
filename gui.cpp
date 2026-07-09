#include "lib/raylib.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>
#include <vector>

// Класс кнопки
class Button {
public:
    Rectangle rect;
    std::string text;
    int textSize;

    Button(float x, float y, float width, float height, std::string s_text = "",
           int text_size = 22) {
        text = s_text;
        rect = {x, y, width, height};
        textSize = text_size;
    }

    bool isClicked() {
        Vector2 mousePos = GetMousePosition();
        return CheckCollisionPointRec(mousePos, rect)
               && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    // Отрисовка кнопки
    void draw() {
        Vector2 mousePos = GetMousePosition();
        int text_w = MeasureText(text.c_str(), textSize);
        bool is_hover = CheckCollisionPointRec(mousePos, rect);
        DrawRectangleRec(rect, is_hover ? LIGHTGRAY : GRAY);
        DrawText(text.c_str(), rect.x + (rect.width - text_w) / 2,
                 rect.y + rect.height / 4, textSize, BLACK);
    }
};

// Класс текстовых полей для цифр
class NumberBox {
public:
    Rectangle rect;
    std::string inputNumber = "0";
    int minN;
    int maxN;
    int number;
    bool isBoxActive = false;

    NumberBox(float x, float y, float width, float height,
              int max_n = 1000, int min_n = 1) {
        rect = {x, y, width, height};
        number = min_n;
        minN = min_n;
        maxN = max_n;
    };

    // Обновление текстового поля
    void update() {
        Vector2 mousePos = GetMousePosition();

        // Вход и выход из текстового поля
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, rect)) {
                SetMouseCursor(MOUSE_CURSOR_IBEAM);
                isBoxActive = true;
            } else {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
                isBoxActive = false;
            }
        }

        // Ввод числа
        if (isBoxActive) {
            int key = GetCharPressed();

            while (key > 0) {
                if (key >= '0' && key <= '9') {
                    if (inputNumber == "0") {
                        inputNumber.pop_back();
                    }
                    inputNumber += (char) key;
                }

                key = GetCharPressed();
            }

            // Стираем символы
            if (IsKeyPressed(KEY_BACKSPACE) && !inputNumber.empty()) {
                inputNumber.pop_back();
                if (inputNumber.empty()) {
                    inputNumber = "0";
                }
            }

            // Завершаем ввод на Enter
            if (IsKeyPressed(KEY_ENTER)) {
                isBoxActive = false;
            }
        }

        // Исправляем число, чтобы оно не выходило за пределы
        number = std::stoi(inputNumber);
        if (number > maxN) {
            number = maxN;
        }
        if (!isBoxActive && number < minN) {
            number = minN;
        }
        inputNumber = std::to_string(number);
    }

    // Отрисовка текстового поля
    void draw() {
        DrawRectangleRec(rect, isBoxActive ? BLUE : LIGHTGRAY);
        int text_w = MeasureText(inputNumber.c_str(), rect.height / 2);
        DrawText(inputNumber.c_str(), rect.x + (rect.width - text_w) / 2,
                 rect.y + rect.height / 4, rect.height / 2, BLACK);
    }
};

class Table
{
private:
    std::vector<std::vector<std::string>> data;
    int currentPage;
    int rows;
    int columns;
    int visibleColumns;

    int cellWidth;
    int cellHeight;

public:
    Table(int columns, int rows, int visibleColumns, int cellWidth, int cellHeight)
    : columns(columns), rows(rows), currentPage(0), visibleColumns(visibleColumns),
    cellHeight(cellHeight), cellWidth(cellWidth)
    {
        currentPage = 0;
        data.clear();
    } 

    int getColumns() {return columns;};
    int getRows() {return rows;};

    void addColumn(std::vector<std::string> columnData)
    {
        if(columnData.size() != rows) throw std::invalid_argument("Incorrect number of rows");
        columns++;
        data.push_back(columnData);
    }

    void setColumn(int index, std::vector<std::string> columnData)
    {
        if(columnData.size() != rows) throw std::invalid_argument("Incorrect number of rows");
        data[index] = columnData;
    }

    void nextPage() 
    {
        if ((currentPage+1)*visibleColumns > columns) return;
        currentPage++;
    }
    void prevPage()
    {
        if (currentPage < 1) return;
        currentPage--;
    }

    void draw(int x, int y)
    {
        for (int row = 0; row < rows; row++)
        {
            for (int i = 0; i < visibleColumns; i++)
            {
                int column = currentPage*visibleColumns + i;

                if (column >= columns)
                    break;

                int Xcell = x + i * cellWidth;
                int Ycell = y + row * cellHeight;

                DrawRectangleLines(Xcell, Ycell, cellWidth, cellHeight, BLACK);

                DrawText(data[column][row].c_str(), Xcell + 5, Ycell + 8, 18, BLACK);
            }
        }
    }

};


// Вариации экрана
enum Screen {
    MENU,
    MTX_CREATION,
    VISUALIZATION
};

// Отображение меню
void menu_screen(Screen &screen, std::vector<NumberBox> &nbs) {
    DrawText("Assignment problem solving", 100, 50, 45, DARKGRAY);
    DrawText("Settings", 500, 150, 40, DARKGRAY);
    DrawText("Number of individuals", 530, 210, 22, DARKGRAY);

    Button btnRandomMtx(60, 180, 320, 60, "Generate random matrix", 26);
    Button btnLoadMtx(60, 280, 320, 60, "Load matrix from file", 26);
    Button btnWriteMtx(60, 380, 320, 60, "Set the matrix", 26);
    Button btnExit(60, 470, 180, 50, "Exit", 26);

    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    // Делаем кнопку светлой, если мышь наведена на неё
    for (auto btn: {btnRandomMtx, btnLoadMtx, btnWriteMtx, btnExit}) {
        btn.draw();
    }

    // Выход из приложения
    if (btnExit.isClicked()) {
        CloseWindow();
    }
    if (btnRandomMtx.isClicked()) {
        // Здесь вызов генерации случайной матрицы
        screen = VISUALIZATION;
    }
    if (btnLoadMtx.isClicked()) {
        // Здесь загружаем нашу матрицу
        screen = VISUALIZATION;
    }
    if (btnLoadMtx.isClicked()) {
        // Здесь загружаем нашу матрицу
        screen = VISUALIZATION;
    }
    if (btnWriteMtx.isClicked()) {
        screen = MTX_CREATION;
    }
}

struct Scaling
{
    int minX;
    int maxX;
    int minY;
    int maxY;
};

// Функция добавления точки из вектора точек
void addPoint(std::vector<Vector2>& points, const GAResult& data, const Rectangle& graphArea, Scaling& scale, int& currStep)
{

    int x = data.history[currStep].generationNumber + 1;
    int y = data.history[currStep].bestCost;


    scale.maxX = currStep + 1;
    scale.minY = std::min(scale.minY, y);
    scale.maxY = std::max(scale.maxY, y);

    float xScale = (graphArea.width - 20) / (float)(scale.maxX - scale.minX + 1);
    float yScale = (graphArea.height - 20) / (float)(scale.maxY - scale.minY + 1);

    points.push_back({0,0});

    // Перерасчет расположения точек
    for(size_t i = 0; i < points.size(); ++i)
    {
        x = data.history[i].generationNumber + 1;
        y = data.history[i].bestCost;

        points[i].x = graphArea.x + 10 + (x - scale.minX) * xScale;
        points[i].y = graphArea.y + graphArea.height - 10 - (y - scale.minY) * yScale;
    }

    if (points.size() == 1)
    {
        points[0].x = graphArea.x + 10 + xScale;
        points[0].y = graphArea.y + graphArea.height - 10 - yScale/2;
    }

    currStep++;
}

// Функция удаления точки из вектора точек
void removePoint(std::vector<Vector2>& points, const GAResult& data, const Rectangle& graphArea, Scaling& scale, int& currStep)
{
    currStep--;
    if(data.history[currStep].bestCost == scale.minY) scale.minY = 10000;
    if(data.history[currStep].bestCost == scale.maxY) scale.maxY = 0;

    points.pop_back();
    

    scale.maxX = currStep;
    scale.minY = std::min(scale.minY, data.history[currStep-1].bestCost);
    scale.maxY = std::max(scale.maxY, data.history[currStep-1].bestCost);

    float xScale = (graphArea.width - 20) / (float)(scale.maxX - scale.minX + 1);
    float yScale = (graphArea.height - 20) / (float)(scale.maxY - scale.minY + 1);

    // Перерасчет расположения точек
    for(size_t i = 0; i < points.size(); ++i)
    {
        int x = data.history[i].generationNumber + 1;
        int y = data.history[i].bestCost;

        points[i].x = graphArea.x + 10 + (x - scale.minX) * xScale;
        points[i].y = graphArea.y + graphArea.height - 10 - (y - scale.minY) * yScale;
    }

    if (points.size() == 1)
    {
        points[0].x = graphArea.x + 10 + xScale;
        points[0].y = graphArea.y + graphArea.height - 10 - yScale/2;
    }
}

void updateTable(Table& table, GAResult& data, int& currStep)
{
    for(size_t i = 1; i < table.getColumns(); ++i)
    {
        std::vector<std::string> column;
        column.push_back(std::to_string(i-1));
        column.push_back(std::to_string(data.history[currStep-1].bestIndividual.chromosome[i-1]));
        column.push_back(std::to_string((i-1)*10));
        table.setColumn(i, column);
    }
}

void visualization_screen(Screen &screen, std::vector<NumberBox> &nbs) {
    // Все полученные промежуточные данные для работы
    GAResult data;

    static int currStep = 0;

    DrawText("Visualization", 100, 50, 30, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);
    Button btnPrevStep(100, 500, 90, 30, "<", 22);
    Button btnNextStep(200, 500, 90, 30, ">", 22);
    Button btnSkipSteps(300, 500, 90, 30, ">>", 22);

    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    for (auto btn: {btnBack, btnNextStep, btnPrevStep, btnSkipSteps}) {
        btn.draw();
    }

    // Отображение лучшего решения
    static Table table(0, 3, 5, 60, 60);

    // Формирование таблицы один раз
    if(currStep != 0 && data.bestIndividual.chromosome.size() > table.getColumns())
    {
        std::vector<std::string> firstColumn;
        firstColumn.push_back("Workr");
        firstColumn.push_back("Work");
        firstColumn.push_back("Cost");
        table.addColumn(firstColumn);
        for (size_t i = 0; i < data.bestIndividual.chromosome.size(); ++i)
        {
            std::vector<std::string> column;
            column.push_back(std::to_string(i));
            column.push_back(std::to_string(data.history[currStep-1].bestIndividual.chromosome[i]));
            column.push_back(std::to_string(i*10));
            table.addColumn(column);
            column.clear();
        }
    }
    DrawText("Current best individual:", 40, 125, 25, DARKGRAY);
    table.draw(40, 150);

    Button btnPrevPage(10, 220, 30, 30, "<", 22);
    Button btnNextPage(340, 220, 30, 30, ">", 22);

    btnPrevPage.draw();
    btnNextPage.draw();

    if(btnNextPage.isClicked())
    {
        table.nextPage();
    }

    if(btnPrevPage.isClicked())
    {
        table.prevPage();
    }

    // Характеристики промежуточного решения
    if(currStep > 0)
    {
        Rectangle paramsArea = {40, 330, 300, 200};
        DrawText(TextFormat("Current step: %d", currStep), paramsArea.x + 10, paramsArea.y + 10, 25, DARKGRAY);
        DrawText(TextFormat("Current best cost: %d", data.history[currStep-1].bestCost), paramsArea.x + 10, paramsArea.y + 35, 25, DARKGRAY);
    }


    // Зона для графика
    Rectangle WholeGraphArea = {400, 0, 400, 600};
    Rectangle graphArea = {440, 0, 360, 560};
    DrawRectangleRec(graphArea, LIGHTGRAY);
    DrawRectangleLinesEx(graphArea, 2, BLACK);
    
    static std::vector<Vector2> points;
    static Scaling scale = {0, 0, 1000000, 0};

    // Минимальные и максимальные значения для масштабирования графика

    if (btnNextStep.isClicked() && currStep < data.history.size()) 
    {
        addPoint(points, data, graphArea, scale, currStep);
        updateTable(table, data, currStep);
    }

    if (btnSkipSteps.isClicked() && currStep < data.history.size()) 
    {
        for(size_t i = currStep; i < data.history.size(); ++i)
        {
            addPoint(points, data, graphArea, scale, currStep);
        }
        updateTable(table, data, currStep);
    }

    if (btnPrevStep.isClicked() && currStep > 1)
    {
        removePoint(points, data, graphArea, scale, currStep);
        updateTable(table, data, currStep);
    }

    // Отрисовка отрезков графика
    for (size_t i = 1; i < points.size(); ++i) {
        DrawLineEx(points[i-1], points[i], 5, RED);
    }

    // Отрисовка точек
    for (size_t i = 0; i < points.size(); ++i) {
        DrawCircleV(points[i], 5, BLACK);
    }

    if (!points.empty())
    {
        DrawCircleV(points.back(), 5, BLUE);
    }

    // Отрисовка чисел на осях
    size_t divisionsNumber = (5 > currStep) ? currStep : 5;
    divisionsNumber = (divisionsNumber < 1) ? 1 : divisionsNumber;

    float xInterval = graphArea.width/(divisionsNumber-1);
    float yInterval = graphArea.height/(divisionsNumber-1);

    float xStepValue = (scale.maxX - scale.minX) / (float)(divisionsNumber - 1);
    float yStepValue = (scale.maxY - scale.minY) / (float)(divisionsNumber - 1);

    float x;
    float y;
    float division;

    // Обработка случая с одной точкой
    if (currStep == 1)
    {
        y = graphArea.y + graphArea.height;
        division = 0;
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x;
        y = points[0].y;
        division = scale.maxY; 
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }

    // Общий случай
    for (size_t i = 0; i < divisionsNumber - 1; ++i)
    {
        // Отрисовка делений по оси абсцисс
        y = graphArea.y + graphArea.height;
        division = std::floor(scale.minX + i*xStepValue);
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x;
        y = graphArea.y + i * yInterval + 10;
        division = scale.maxY - i * yStepValue; 
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }

    // Отрисовка крайних чисел, т.к. вылезают за границы рамки
    if (divisionsNumber > 1)
    {
        y = graphArea.y + graphArea.height;
        division = std::floor(scale.minX + (divisionsNumber-1)*xStepValue - 1);
        x = points[division].x;
        DrawText(TextFormat("%d", division), x, y, 20, DARKGRAY);

        // Отрисовка делений по оси ординат
        x = WholeGraphArea.x;
        y = graphArea.y + (divisionsNumber-1) * yInterval - 20;
        division = scale.maxY - (divisionsNumber-1) * yStepValue; 
        DrawText(TextFormat("%d", int(division)), x, y, 20, DARKGRAY);
    }
    
    if (btnBack.isClicked()) {
        screen = MENU;
        currStep = 0;
        points.clear();
        table = Table(0, 3, 5, 60, 60);
    }
}

void mtx_creation_screen(Screen &screen, std::vector<NumberBox> &nbs) {
    DrawText("Matrix creation", 100, 50, 30, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);

    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    for (auto btn: {btnBack}) {
        btn.draw();
    }

    if (btnBack.isClicked()) {
        // Отменяем создание матрицы
        screen = MENU;
    }
}

int main() {
    // Инициализация окна
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "GUI prototype");
    SetTargetFPS(60);
    Screen screen = MENU; // Изначальный экран - меню

    std::vector<NumberBox> menu_nb = {NumberBox(460, 200, 60, 40)};
    std::vector<NumberBox> vis_nb = {};
    std::vector<NumberBox> mc_nb = {};

    // Главный цикл программы (работает, пока не закроют окно)
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (screen) {
            case MENU:
                menu_screen(screen, menu_nb);
                break;
            case VISUALIZATION:
                visualization_screen(screen, vis_nb);
                break;
            case MTX_CREATION:
                mtx_creation_screen(screen, mc_nb);
                break;
            default:
                screen = MENU;
                std::cerr << "Error: Switching to an unknown screen" << std::endl;
                break;
        }

        EndDrawing();
    }

    // Закрытие окна
    CloseWindow();
    return 0;
}
