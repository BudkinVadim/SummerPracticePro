#include  "gui_logic/GUI.h"

#include <iostream>
#include <string>
#include <vector>


int main() {
    // Инициализация окна

    GUI gui;

    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Summer practice work");
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

    // Ячейка для выбора размера матрицы для генерации
    menu_nb.emplace_back(380, 180, 60, 60);
    menu_nb.back().minN = 2;
    menu_nb.back().maxN = 1000;
    menu_nb.back().setNumber(10);


    std::vector<NumberBox> vis_nb;
    vis_nb.emplace_back(-10, -10, 0, 0, 10, 0);

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

        if (gui.gaRunning && gui.gaFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            gui.gaRunning = false;
            gui.screen = VISUALIZATION;   // переключаем на визуализацию
        }
        switch (gui.screen) {
            case EXIT:
                break;
            case MENU:
                gui.menu_screen(menu_nb, errorMessage);
                break;
            case LOADING:
                gui.loading_screen(menu_nb, errorMessage);
                break;
            case VISUALIZATION:
                gui.visualization_screen(vis_nb);
                break;
            case MTX_CREATION:
                gui.mtx_creation_screen(mtx_nb, errorMessage);
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
