#include "lib/raylib.h"
#include "lib/tinyfiledialogs.h"
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

// Вариации экрана
enum Screen {
    EXIT,
    MENU,
    MTX_CREATION,
    VISUALIZATION,
};

// Отображение меню
void menu_screen(Screen &screen, std::vector<NumberBox> &nbs, std::string &errorMessage) {
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
        // Тут мы должны передать данные для ГА
        screen = VISUALIZATION;
    }
}

int main() {
    // Инициализация окна
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "GUI prototype");
    SetTargetFPS(60);
    Screen screen = MENU; // Изначальный экран - меню

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
    while (!WindowShouldClose() && screen != EXIT) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (screen) {
            case EXIT:
                break;
            case MENU:
                menu_screen(screen, menu_nb, errorMessage);
                break;
            case VISUALIZATION:
                visualization_screen(screen, vis_nb);
                break;
            case MTX_CREATION:
                mtx_creation_screen(screen, mtx_nb);
                break;
            default:
                std::cerr << "Error: Switching to an unknown screen" << std::endl;
                screen = MENU;
                menu_screen(screen, menu_nb, errorMessage);
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
