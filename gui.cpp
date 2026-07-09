#include "lib/raylib.h"
#include <iostream>
#include <algorithm>
#include <cmath>
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

void visualization_screen(Screen &screen, std::vector<NumberBox> &nbs) {
    // Все полученные промежуточные данные для работы
    GAResult data;

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

    // Зона для графика
    Rectangle WholeGraphArea = {400, 0, 400, 600};
    Rectangle graphArea = {440, 0, 360, 560};
    DrawRectangleRec(graphArea, LIGHTGRAY);
    DrawRectangleLinesEx(graphArea, 2, BLACK);
    
    static int currStep = 0;
    static std::vector<Vector2> points;
    static Scaling scale = {0, 0, 1000000, 0};

    // Минимальные и максимальные значения для масштабирования графика

    if (btnNextStep.isClicked() && currStep < data.history.size()) 
    {
        addPoint(points, data, graphArea, scale, currStep);
    }

    if (btnSkipSteps.isClicked() && currStep < data.history.size()) 
    {
        for(size_t i = currStep; i < data.history.size(); ++i)
        {
            addPoint(points, data, graphArea, scale, currStep);
        }
    }

    if (btnPrevStep.isClicked() && currStep > 1)
    {
        removePoint(points, data, graphArea, scale, currStep);
    }

    // Отрисовка отрезков графика
    for (size_t i = 1; i < points.size(); ++i) {
        DrawLineEx(points[i-1], points[i], 5, RED);
    }

    // Отрисовка точек
    for (size_t i = 0; i < points.size(); ++i) {
        DrawCircleV(points[i], 5, BLACK);
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

    if (!points.empty())
    {
        DrawCircleV(points.back(), 5, BLUE);
    }


    // Отрисовка чисел относительно масштаба
    if (btnBack.isClicked()) {
        screen = MENU;
        currStep = 0;
        points.clear();
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
