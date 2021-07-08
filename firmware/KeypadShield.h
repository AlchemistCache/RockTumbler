#pragma once

#include "Arduino.h"

//кнопки на LCD Keypad Shield 1602
#define BTN_UP 1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 0

#define NODELAY 250 //время для устранения дребезга кнопок

uint32_t _buttonTimer = 0; //таймер для детекирования нажатия кнопки

uint8_t detectButton() //детектируем нажатие кнопки
{
    uint16_t keyAnalog = analogRead(A0);
    uint8_t message = BTN_NONE; //значение сообщения по умолчанию
    if (millis() - _buttonTimer >= NODELAY)
    {
        if (keyAnalog < 100)
        { // Значение меньше 100 – нажата кнопка right
            message = BTN_RIGHT;
        }
        else if (keyAnalog < 200)
        { // Значение больше 100 (иначе вошли бы в предыдущий блок сравнения) но меньше 200 – нажата кнопка UP
            message = BTN_UP;
        }
        else if (keyAnalog < 400)
        { // Значение больше 200, но меньше 400 – нажата кнопка DOWN
            message = BTN_DOWN;
        }
        else if (keyAnalog < 600)
        { // Значение больше 400, но меньше 600 – нажата кнопка LEFT
            message = BTN_LEFT;
        }
        else if (keyAnalog < 800)
        { // Значение больше 600, но меньше 800 – нажата кнопка SELECT
            message = BTN_SELECT;
        }
        _buttonTimer = millis();
    }
    /*if (message != BTN_NONE)
    {    }*/
    return message;
}