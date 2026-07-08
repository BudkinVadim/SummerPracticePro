#include "lib/raylib.h"
#include <iostream>
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

void visualization_screen(Screen &screen, std::vector<NumberBox> &nbs) {
    DrawText("Visualization", 100, 50, 30, DARKGRAY);
    Button btnBack(10, 5, 90, 30, "Back", 22);

    for (NumberBox &nb: nbs) {
        nb.update();
        nb.draw();
    }

    for (auto btn: {btnBack}) {
        btn.draw();
    }

    if (btnBack.isClicked()) {
        // Останавливаем работу алгоритма
        screen = MENU;
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
