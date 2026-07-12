// #include "gui.h"
#include "lib/raylib.h"
#include "lib/tinyfiledialogs.h"
#include "genetic/GeneticAlgorithm.h"
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
    bool firstTouch = true;

    NumberBox(float x, float y, float width, float height,
              int max_n = 1000, int min_n = 1) {
        rect = {x, y, width, height};
        number = min_n;
        minN = min_n;
        maxN = max_n;
    };

    void setNumber(int n) {
        number = n;
        inputNumber = std::to_string(n);
    }

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
                firstTouch = true;
            }
        }

        // Ввод числа
        if (isBoxActive) {
            int key = GetCharPressed();

            while (key > 0) {
                if (key >= '0' && key <= '9') {
                    if (firstTouch) {
                        inputNumber = "0";
                        firstTouch = false;
                    }
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

            // Исправляем число, чтобы оно не выходило за пределы
            int tmpNumber = std::stoi(inputNumber);
            if (tmpNumber > maxN) {
                tmpNumber = maxN;
            }
            inputNumber = std::to_string(tmpNumber);
        } else {
            number = std::stoi(inputNumber);
            if (number > maxN) {
                number = maxN;
            }
            if (!isBoxActive && number < minN) {
                number = minN;
            }
            inputNumber = std::to_string(number);
        }
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


enum Screen {
    EXIT,
    MENU,
    MTX_CREATION,
    VISUALIZATION,
};

// Вариации экрана
class GUI
{
public:

    // Текущая вариация экрана
    Screen screen;

    // Данные алгоритма
    std::vector<std::vector<int>> costMatrix;
    GASettings settings;
    GAResult result;

    void getGAResult()
    {
        GeneticAlgorithm ga;
        result = ga.run(costMatrix, settings);
    }

    // Отображение меню
    void menu_screen(std::vector<NumberBox> &nbs, std::string &errorMessage) {
        DrawText("Assignment problem solving", 100, 50, 45, DARKGRAY);
        DrawText("Settings", 500, 130, 40, DARKGRAY);
        DrawText("Individuals", 530, 190, 22, DARKGRAY);
        DrawText("Elite individuals", 530, 231, 22, DARKGRAY);
        DrawText("Max generations", 530, 272, 22, DARKGRAY);
        DrawText("Max stagnation", 530, 313, 22, DARKGRAY);
        DrawText("Crossing chance", 530, 354, 22, DARKGRAY);
        DrawText("Mutation chance", 530, 395, 22, DARKGRAY);

        nbs[1].maxN = nbs[0].number;
        nbs[3].maxN = nbs[2].number;
        nbs[9].maxN = nbs[0].number;

        Button btnRandomMtx(60, 180, 320, 60, "Generate random matrix", 26);
        Button btnLoadMtx(60, 280, 320, 60, "Load matrix from file", 26);
        Button btnWriteMtx(60, 380, 320, 60, "Set the matrix", 26);
        Button btnExit(60, 470, 180, 50, "Exit", 26);

        std::string mutation = "Mutation: ";
        std::string mut_var[] = {"Exchange", "Subarray inversion"};
        mutation += mut_var[nbs[6].number];

        std::string crossing = "Crossing: ";
        std::string cros_var[] = {"Ordered", "Positional"};
        crossing += cros_var[nbs[7].number];

        std::string selection = "Selection: ";
        std::string sel_var[] = {"Tournament", "Stochastic Roulette"};
        selection += sel_var[nbs[8].number];

        Button btnMutation(440, 435, 340, 40, mutation, 22);
        Button btnCrossing(440, 480, 240, 40, crossing, 22);
        Button btnSelection(380, 525, 340, 40, selection, 22);

        for (NumberBox &nb: nbs) {
            nb.update();
            nb.draw();
        }

        // Делаем кнопку светлой, если мышь наведена на неё
        for (auto btn: {
                btnRandomMtx, btnLoadMtx, btnWriteMtx, btnExit,
                btnMutation, btnCrossing, btnSelection
            }) {
            btn.draw();
        }

        // Переключение режимов мутации, скрещивания и отбора
        if (btnMutation.isClicked()) {
            nbs[6].setNumber((nbs[6].number + 1) % 2);
        }
        if (btnCrossing.isClicked()) {
            nbs[7].setNumber((nbs[7].number + 1) % 2);
        }
        if (btnSelection.isClicked()) {
            nbs[8].setNumber((nbs[8].number + 1) % 2);
        }

        // Передача настроек в настройки алгоритма
        settings.populationSize = nbs[0].number;
        settings.eliteCount = nbs[1].number;
        settings.maxGenerationsWithoutImprovement = nbs[2].number;
        settings.crossoverProbability = nbs[4].number;
        settings.mutationProbability = nbs[5].number;
        
        settings.mutationType = static_cast<MutationType>(nbs[6].number);
        settings.crossoverType = static_cast<CrossoverType>(nbs[7].number);
        settings.selectionType = static_cast<SelectionType>(nbs[8].number);

        // Выход из приложения
        if (btnExit.isClicked()) {
            screen = EXIT;
            return;
        }

        if (btnRandomMtx.isClicked()) {
            // Здесь вызов генерации случайной матрицы
            screen = VISUALIZATION;
        }
        if (btnLoadMtx.isClicked()) {
            const char *filter[] = {"*.csv"};
            const char *selectPath = tinyfd_openFileDialog(
                "Выберите файл с матрицей", "",
                1, filter,
                "*.csv", 0
            );

            if (selectPath != nullptr) {
                std::string pathString = selectPath;
                std::cout << pathString << std::endl;
                // Дальше запускаем функцию загрузки и валидации матрицы в ГА
                bool someError = true;
                if (someError) {
                    errorMessage = "Invalid file format";
                } else {
                    screen = VISUALIZATION;
                }
            } else {
                errorMessage = "";
            }
        }
        if (btnWriteMtx.isClicked()) {
            screen = MTX_CREATION;
        }

        if (screen != MENU) {
            errorMessage = "";
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
    void addPoint(std::vector<Vector2>& points, const Rectangle& graphArea, Scaling& scale, int& currStep)
    {

        int x = result.history[currStep].generationNumber + 1;
        int y = result.history[currStep].bestCost;


        scale.maxX = currStep + 1;
        scale.minY = std::min(scale.minY, y);
        scale.maxY = std::max(scale.maxY, y);

        float xScale = (graphArea.width - 20) / (float)(scale.maxX - scale.minX + 1);
        float yScale = (graphArea.height - 20) / (float)(scale.maxY - scale.minY + 1);

        points.push_back({0,0});

        // Перерасчет расположения точек
        for(size_t i = 0; i < points.size(); ++i)
        {
            x = result.history[i].generationNumber + 1;
            y = result.history[i].bestCost;

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
    void removePoint(std::vector<Vector2>& points, const Rectangle& graphArea, Scaling& scale, int& currStep)
    {
        currStep--;
        if(result.history[currStep].bestCost == scale.minY) scale.minY = 10000;
        if(result.history[currStep].bestCost == scale.maxY) scale.maxY = 0;

        points.pop_back();
        

        scale.maxX = currStep;
        scale.minY = std::min(scale.minY, result.history[currStep-1].bestCost);
        scale.maxY = std::max(scale.maxY, result.history[currStep-1].bestCost);

        float xScale = (graphArea.width - 20) / (float)(scale.maxX - scale.minX + 1);
        float yScale = (graphArea.height - 20) / (float)(scale.maxY - scale.minY + 1);

        // Перерасчет расположения точек
        for(size_t i = 0; i < points.size(); ++i)
        {
            int x = result.history[i].generationNumber + 1;
            int y = result.history[i].bestCost;

            points[i].x = graphArea.x + 10 + (x - scale.minX) * xScale;
            points[i].y = graphArea.y + graphArea.height - 10 - (y - scale.minY) * yScale;
        }

        if (points.size() == 1)
        {
            points[0].x = graphArea.x + 10 + xScale;
            points[0].y = graphArea.y + graphArea.height - 10 - yScale/2;
        }
    }

    void updateTable(Table& table, int& currStep)
    {
        for(size_t i = 1; i < table.getColumns(); ++i)
        {
            std::vector<std::string> column;
            int workId = result.history[currStep-1].bestIndividual.chromosome[i-1];
            column.push_back(std::to_string(i-1));
            column.push_back(std::to_string(workId));
            column.push_back(std::to_string(costMatrix[i-1][workId])) ;
            table.setColumn(i, column);
        }
    }

    void visualization_screen(std::vector<NumberBox> &nbs) {
        // Получение данных для визуализации
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
        if(currStep != 0 && result.bestIndividual.chromosome.size() > table.getColumns())
        {
            std::vector<std::string> firstColumn;
            firstColumn.push_back("Workr");
            firstColumn.push_back("Work");
            firstColumn.push_back("Cost");
            table.addColumn(firstColumn);
            for (size_t i = 0; i < result.bestIndividual.chromosome.size(); ++i)
            {
                std::vector<std::string> column;
                int workId = result.history[currStep-1].bestIndividual.chromosome[i];
                column.push_back(std::to_string(i));
                column.push_back(std::to_string(workId));
                column.push_back(std::to_string(costMatrix[i][workId]));
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
            DrawText(TextFormat("Current best cost: %d", result.history[currStep-1].bestCost), paramsArea.x + 10, paramsArea.y + 35, 25, DARKGRAY);
        }


        // Зона для графика
        Rectangle WholeGraphArea = {400, 0, 400, 600};
        Rectangle graphArea = {440, 0, 360, 560};
        DrawRectangleRec(graphArea, LIGHTGRAY);
        DrawRectangleLinesEx(graphArea, 2, BLACK);
        
        static std::vector<Vector2> points;
        static Scaling scale = {0, 0, 1000000, 0};

        // Минимальные и максимальные значения для масштабирования графика

        if (btnNextStep.isClicked() && currStep < result.history.size()) 
        {
            addPoint(points, graphArea, scale, currStep);
            updateTable(table, currStep);
        }

        if (btnSkipSteps.isClicked() && currStep < result.history.size()) 
        {
            for(size_t i = currStep; i < result.history.size(); ++i)
            {
                addPoint(points, graphArea, scale, currStep);
            }
            updateTable(table, currStep);
        }

        if (btnPrevStep.isClicked() && currStep > 1)
        {
            removePoint(points, graphArea, scale, currStep);
            updateTable(table, currStep);
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

    void mtx_creation_screen(std::vector<NumberBox> &nbs) {
        DrawText("Cost matrix creation", 100, 50, 30, DARKGRAY);
        Button btnBack(10, 5, 90, 30, "Back", 22);
        Button btnContinue(630, 500, 160, 60, "Continue", 26);

        DrawText("Matrix size", 130, 110, 22, DARKGRAY);

        int mtxSize = nbs.back().number;

        DrawText("Works", 365, 365 - 41 * (mtxSize / 2.f + 1), 22, DARKGRAY);
        DrawText("W\no\nr\nk\ne\nr\ns",
                385 - 41 * (mtxSize / 2.f + 1), 295, 22, DARKGRAY);

        nbs.back().update();
        nbs.back().draw();

        int makeNextActive = 0;
        for (int i = -1; i < mtxSize; i++) {
            for (int j = -1; j < mtxSize; j++) {
                Rectangle pos = {
                    400 - 41 * (mtxSize / 2.f - j),
                    380 - 41 * (mtxSize / 2.f - i), 40, 40
                };

                if (i >= 0 && j >= 0) {
                    NumberBox &nb = nbs[j + i * 10];
                    if (nb.isBoxActive && IsKeyPressed(KEY_ENTER)) {
                        makeNextActive = 2;
                    }
                    nb.rect = pos;
                    nb.update();
                    if (makeNextActive == 2) {
                        makeNextActive--;
                    } else if (makeNextActive == 1) {
                        nb.isBoxActive = true;
                        makeNextActive--;
                    }
                    nb.draw();
                    continue;
                }

                // Отображение номеров столбцов и колонок
                char c[] = {'0'};
                if (i == -1 && j != -1) {
                    c[0] += j;
                    DrawText(c, pos.x + 15, pos.y + 15, 22, LIGHTGRAY);
                }
                if (i != -1 && j == -1) {
                    c[0] += i;
                    DrawText(c, pos.x + 15, pos.y + 10, 22, LIGHTGRAY);
                }
            }
        }

        for (auto btn: {btnBack, btnContinue}) {
            btn.draw();
        }

        if (btnBack.isClicked()) {
            // Отменяем создание матрицы
            screen = MENU;
        }
        if (btnContinue.isClicked()) {
            // Передача данных из полей в матрицу
            costMatrix = std::vector<std::vector<int>>(mtxSize, std::vector<int>(mtxSize));
            for (size_t i = 0; i < mtxSize; ++i)
            {
                for (size_t j = 0; j < mtxSize; ++j)
                {
                    costMatrix[i][j] = nbs[i*mtxSize + j].number;
                }
            }
            getGAResult();
            screen = VISUALIZATION;
        }
    }

};  

int main() {
    // Инициализация окна

    GUI gui;

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "GUI prototype");
    SetTargetFPS(60);
    gui.screen = MENU; // Изначальный экран - меню

    std::vector<NumberBox> menu_nb;
    for (int i = 0; i < 6; i++) {
        menu_nb.emplace_back(460, 180 + i * 41, 60, 40);
    }
    menu_nb[0].setNumber(100); // количество особей
    menu_nb[1].setNumber(5); // количество элитных особей
    menu_nb[2].setNumber(500); // максимальное количество поколений
    menu_nb[3].setNumber(50); // максимальное количество поколений без прогресса
    menu_nb[4].maxN = 100; // шанс скрещивания
    menu_nb[4].setNumber(50);
    menu_nb[4].minN = 0;
    menu_nb[5].maxN = 100; // шанс мутации
    menu_nb[5].setNumber(50);
    menu_nb[5].minN = 0;

    // Ещё три невидимых поля для выбора метода мутации, скрещивания и отбора
    for (int i = 0; i < 3; i++) {
        menu_nb.emplace_back(-10, -10, 0, 0, 10, 0);
    }
    menu_nb.emplace_back(720, 525, 60, 40); // Ячейка для значений отбора
    menu_nb.back().setNumber(5);

    std::vector<NumberBox> vis_nb = {};
    std::vector<NumberBox> mtx_nb;
    for (int i = 0; i < 100; i++) {
        mtx_nb.emplace_back(380, 280, 40, 40);
    }
    mtx_nb.emplace_back(60, 100, 60, 40, 10);
    mtx_nb.back().setNumber(10);

    std::string errorMessage;

    // Главный цикл программы (работает, пока не закроют окно)
    while (!WindowShouldClose() && gui.screen != EXIT) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (gui.screen) {
            case EXIT:
                break;
            case MENU:
                gui.menu_screen(menu_nb, errorMessage);
                break;
            case VISUALIZATION:
                gui.visualization_screen(vis_nb);
                break;
            case MTX_CREATION:
                gui.mtx_creation_screen(mtx_nb);
                break;
            default:
                std::cerr << "Error: Switching to an unknown screen" << std::endl;
                gui.screen = MENU;
                gui.menu_screen(menu_nb, errorMessage);
                break;
        }
        if (!errorMessage.empty()) {
            DrawText(errorMessage.c_str(), 40, 550, 30, RED);
        }
        EndDrawing();
    }

    // Закрытие окна
    CloseWindow();
    return 0;
}
