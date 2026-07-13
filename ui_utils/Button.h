#ifndef SP_WORK_BUTTON_H
#define SP_WORK_BUTTON_H
#include "../lib/raylib.h"
#include <string>

// Класс кнопки
class Button {
public:
    Rectangle rect;
    std::string text;
    int textSize;

    Button(float x, float y, float width, float height, std::string s_text = "",
           int text_size = 22);

    // Обработка нажатия
    bool isClicked();

    // Отрисовка кнопки
    void draw();
};

#endif //SP_WORK_BUTTON_H
