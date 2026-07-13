#include "NumberBox.h"

NumberBox::NumberBox(float x, float y, float width, float height,
                     int max_n, int min_n) {
    rect = {x, y, width, height};
    number = min_n;
    minN = min_n;
    maxN = max_n;
};

void NumberBox::setNumber(int n) {
    number = n;
    inputNumber = std::to_string(n);
}

void NumberBox::update() {
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

void NumberBox::draw() {
    DrawRectangleRec(rect, isBoxActive ? BLUE : LIGHTGRAY);
    int text_w = MeasureText(inputNumber.c_str(), rect.height / 2);
    DrawText(inputNumber.c_str(), rect.x + (rect.width - text_w) / 2,
             rect.y + rect.height / 4, rect.height / 2, BLACK);
}
