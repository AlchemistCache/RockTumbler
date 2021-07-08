#pragma once
#include <Arduino.h>

#define NUM_OF(x) (sizeof(x) / sizeof(x[0])) //Макрос для высчитывания количества режимов в структуре rejim (по умолчанию три)

struct rejimStruct				 //все настройки режима, у каждого режима эти настройки свои
{
	uint8_t hour;
	uint8_t minute;
	uint8_t speedRot;
};

rejimStruct rejim[3];           //общее кол-во режимов

uint8_t currentActiveRegime = 0; //текущий активный режим. (Здесь задано значение по-умолчанию при старте)
