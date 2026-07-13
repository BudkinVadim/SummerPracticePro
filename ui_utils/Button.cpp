#include "Button.h"

Button::Button(float x, float y, float width, float height, std::string s_text,
               int text_size) {
    text = s_text;
    rect = {x, y, width, height};
    textSize = text_size;
}

// Обработка нажатия
bool Button::isClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, rect)
           && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Отрисовка кнопки
void Button::draw() {
    Vector2 mousePos = GetMousePosition();
    int text_w = MeasureText(text.c_str(), textSize);
    bool is_hover = CheckCollisionPointRec(mousePos, rect);
    DrawRectangleRec(rect, is_hover ? LIGHTGRAY : GRAY);
    DrawText(text.c_str(), rect.x + (rect.width - text_w) / 2,
             rect.y + rect.height / 4, textSize, BLACK);
}
