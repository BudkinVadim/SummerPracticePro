#ifndef SP_WORK_NUMBERBOX_H
#define SP_WORK_NUMBERBOX_H
#include "../lib/raylib.h"
#include <string>

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
              int max_n = 1000, int min_n = 1);

    // Задать число
    void setNumber(int n);

    // Обновление текстового поля
    void update();

    // Отрисовка текстового поля
    void draw();
};

#endif //SP_WORK_NUMBERBOX_H
