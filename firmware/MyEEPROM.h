#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define INIT_ADDR 1023 //номер ячейки для определения первого запуска
#define INIT_KEY 254     //ключ первого запуска. 0-254

rejimStruct EEMEM myStructAddr; //указатель на адрес структуры

void updateEEPROM()
{
    EEPROM.put((uint8_t)&myStructAddr, rejim);
}

void readEEPROM()
{
    if (EEPROM.read(INIT_ADDR) != INIT_KEY) //если первый запуск программы
    {
        EEPROM.put(INIT_ADDR, INIT_KEY);    // обновим ключ

        rejim[0] = {0, 01, 5};
        rejim[1] = {10, 00, 1};
        rejim[2] = {5, 30, 4};

        //return true;
    }

    else
    {
        EEPROM.get((uint8_t)&myStructAddr, rejim);
    }
}